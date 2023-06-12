#include <st_d3d.h>

#include "main.h"

#include <math.h>

#include <ninja.h>
#include <ne_lib.h>
#include <sg_syhw.h>
#include <st_d3d_texture.h>

// Libraries
#pragma comment(lib, "d3d8.lib")

// Direct3D constants
const D3DFORMAT d3d_pref_format[4] = {D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_A1R5G5B5, D3DFMT_R5G6B5};
const int d3d_pref_format_count = 4;

const D3DFORMAT d3d_depthstencil_format_def[5] = {D3DFMT_D16, D3DFMT_D15S1, D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D24X4S4};
const D3DFORMAT d3d_depthstencil_format_high[5] = {D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D24X4S4, D3DFMT_D16, D3DFMT_D15S1};

const D3DFORMAT *d3d_depthstencil_format = d3d_depthstencil_format_def;
const int d3d_depthstencil_format_count = 5;

// Direct3D globals
int d3d_windowed = 1;
int d3d_screen_width = 640;
int d3d_screen_height = 480;

int st_d3d_flag;

IDirect3D8 *d3d_d3d8 = NULL;

UINT d3d_adapter_id;

IDirect3DDevice8 *d3d_device = NULL;
D3DPRESENT_PARAMETERS d3d_present_parameters;
D3DCAPS8 d3d_caps;

D3DVIEWPORT8 d3d_viewport;
float d3d_viewcenterp[2];
float d3d_viewrect[4];

D3DMATRIX d3d_unitmatrix = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
D3DMATRIX d3d_view;

// Direct3D control
DWORD d3d_ctrlflag0;
DWORD d3d_ctrlflag1;
DWORD d3d_ctrlflag2;
DWORD d3d_ctrlflag3;

void stGetCtrlFlag(void)
{
	d3d_ctrlflag0 = 0;
	d3d_ctrlflag1 = 0;
	d3d_ctrlflag2 = 0;
	d3d_ctrlflag3 = 0;
	neCtrlFlag(TEXT("CTRLFLAG0"), (BYTE*)&d3d_ctrlflag0);
	neCtrlFlag(TEXT("CTRLFLAG1"), (BYTE*)&d3d_ctrlflag1);
	neCtrlFlag(TEXT("CTRLFLAG2"), (BYTE*)&d3d_ctrlflag2);
	neCtrlFlag(TEXT("CTRLFLAG3"), (BYTE*)&d3d_ctrlflag3);
}

BOOL stTestCtrlFlag0(DWORD flag)
{
	return (d3d_ctrlflag0 & flag) != 0;
}

// Direct3D window functions
LRESULT CALLBACK stWindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CANCELMODE:
			if (d3d_windowed)
			{
				// Exit program
				neExit();
			}
			break;
	}
	return DefWindowProc(hwnd, message, wparam, lparam);
}

void stWindowCreate(HINSTANCE hinst, int show_cmd)
{
	// Register window class
	WNDCLASS wnd_class;
	wnd_class.style = 0;
	wnd_class.lpfnWndProc = stWindowProc;
	wnd_class.cbClsExtra = 0;
	wnd_class.cbWndExtra = 0;
	wnd_class.hInstance = hinst;
	wnd_class.hIcon = LoadIcon(0, IDI_APPLICATION);
	wnd_class.hCursor = LoadCursor(0, IDI_APPLICATION);
	wnd_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnd_class.lpszMenuName = 0;
	wnd_class.lpszClassName = stQueryAppName();

	if (RegisterClass(&wnd_class) == 0)
		return;

	// Create window
	DWORD window_exstyle, window_style;
	if (d3d_windowed)
	{
		window_exstyle = 0;
		window_style = WS_CAPTION | WS_SYSMENU;
	}
	else
	{
		window_exstyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
		window_style = WS_CAPTION;
	}

	main_hwnd = CreateWindowEx(window_exstyle, stQueryAppName(), stQueryAppName(), window_style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hinst, NULL);
	ShowWindow(main_hwnd, show_cmd);
    UpdateWindow(main_hwnd);
    SetForegroundWindow(main_hwnd);
}

void stWindowInit(void)
{
	// Create window
	stWindowCreate(main_hinst, main_showcmd);

	// Run window initialization functions
	stGetCtrlFlag();

	// Show cursor and push window to front despite show command
	ShowCursor(TRUE);
	SetForegroundWindow(main_hwnd);
}

// Direct3D fog functions
BOOL st_fog_enable;
BOOL st_fog_support;

int st_fog_color;
int st_fog_range_start;
int st_fog_range_end;

float st_fog_emu_depth;

float st_fog_range[128];
float st_fog_range_base[128];

float st_fog_density_start;
float st_fog_density_end;

void stFogDensity(unsigned short density)
{
	// Setup fog density range
	float v1 = v1 = (float)(1 << (density & 0x1F));
	float v3 = 1.0f / (v1 * ((float)(density & 0xFF00) * (1.0f / 65536.0f)));

	for (int i = 0; i < 128; i++)
	{
		float v4 = v3 * st_fog_range_base[i];
		if (v4 < (1.0f / 65536.0f))
			st_fog_range[i] = 65536.0f;
		else
			st_fog_range[i] = 1.0f / v4;
	}

	// Setup density state
	float range_start = st_fog_range[st_fog_range_start];
	float range_end = st_fog_range[st_fog_range_end];

	float density_range = st_fog_density_end - st_fog_density_start;
	float range = range_end - range_start;

	// Setup fog emulation
	if (!st_fog_support)
	{
		if (st_fog_density_end >= 0.9f)
		{
			if (density_range > 0.0f)
			{
				if (range / density_range * (1.0f - st_fog_density_end) + range_end >= 100.0f)
					st_fog_emu_depth = 1.0f;
				else
					st_fog_emu_depth = 0.3f;
				return;
			}
			if (st_fog_density_start < 0.3f)
			{
				st_fog_emu_depth = 1.0f;
				return;
			}
		}
		else
		{
			if (density_range > 1.0f)
			{
				if (st_fog_density_start < 0.5)
					st_fog_emu_depth = 0.3f;
				else
					st_fog_emu_depth = 1.0f;
				return;
			}
			if (st_fog_density_start < 0.3f)
			{
				st_fog_emu_depth = 1.0f;
				return;
			}
		}
		if (st_fog_density_start <= 0.3f || st_fog_density_start >= 0.6f)
			st_fog_emu_depth = 0.3f;
		else
			st_fog_emu_depth = 0.6f;
		return;
	}

	// Setup hardware fog
	if (st_fog_density_end >= 0.9f)
	{
		float fog_start, fog_end;
		if (density_range <= 0.0f)
		{
			if (st_fog_density_start >= 0.3f)
			{
				if (st_fog_density_start <= 0.3f || st_fog_density_start >= 0.6f)
				{
					fog_start = 0.0f;
					fog_end = 100.0f;
				}
				else
				{
					fog_start = 0.0f;
					fog_end = 32767.0f;
				}
			}
			else
			{
				fog_start = 65534.0f;
				fog_end = 65535.0f;
			}
		}
		else
		{
			fog_start = (-st_fog_density_start * (range / density_range)) + range_start;
			fog_end = ((1.0f - st_fog_density_end) * (range / density_range)) + range_end;
			if (fog_start < 0.0f)
				fog_start = 0.0f;
			if (fog_end > 65535.0f)
				fog_end = 65535.0f;
		}

		d3d_device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
		d3d_device->SetRenderState(D3DRS_FOGSTART, *((DWORD*)&fog_start));
		d3d_device->SetRenderState(D3DRS_FOGEND, *((DWORD*)&fog_end));
	}
	else if (density_range <= 1.0f)
	{
		float fog_start, fog_end;
		if (st_fog_density_start >= 0.3f)
		{
			if (st_fog_density_start <= 0.3f || st_fog_density_start >= 0.6f)
				fog_end = 100.0f;
			else
				fog_end = 32767.0f;
			fog_start = 0.0f;
		}
		else
		{
			fog_start = 65534.0f;
			fog_end = 65535.0f;
		}

		d3d_device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
		d3d_device->SetRenderState(D3DRS_FOGSTART, *((DWORD*)&fog_start));
		d3d_device->SetRenderState(D3DRS_FOGEND, *((DWORD*)&fog_end));
	}
	else if (st_fog_density_start < 0.5f)
	{
		float fog_density = sqrt(log(-((density_range * 0.5f) - 1.0f)) * -1.0f) * (1.0f / ((range_end + range_start) * 0.5f));
		d3d_device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP2);
		d3d_device->SetRenderState(D3DRS_FOGDENSITY, *((DWORD*)&fog_density));
	}
	else
	{
		d3d_device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
		d3d_device->SetRenderState(D3DRS_FOGSTART, 0x00000000); // 0.0f
		d3d_device->SetRenderState(D3DRS_FOGEND, 0x42c80000); // 100.0f
	}
}

void stFogInit(void)
{
	// Disable fog
	st_fog_enable = FALSE;
	d3d_device->SetRenderState(D3DRS_FOGENABLE, 0);

	// Test fog support
	st_fog_support = (d3d_caps.RasterCaps & D3DPRASTERCAPS_WFOG) != 0;
	if (stTestCtrlFlag0(0x20))
		st_fog_support = 0;

	if (st_fog_support)
	{
		// Enable hardware fog
		d3d_device->SetRenderState(D3DRS_FOGCOLOR, 0xFFFFFF);
		d3d_device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
		d3d_device->SetRenderState(D3DRS_FOGSTART, 0x00000000); // 0.0f
		d3d_device->SetRenderState(D3DRS_FOGEND, 0x477FFF00); // 65535.0f
	}
	else
	{
		// Setup fog emulation
		st_fog_emu_depth = 1.0f;
	}

	// Initialize fog state
	st_fog_color = 0xFFFFFF;
	st_fog_range_start = 0x7E;
	st_fog_range_end = 0x7F;

	// Initialize fog density
	for (int i = 0; i < 128; i++)
	{
		long double v1 = pow(2.0, (double)((i >> 4) & 0xF));
		int v2 = i & 0xF;
		st_fog_range_base[i] = (float)(v1 * ((double)v2 + 16.0) * 0.0625);
	}
	stFogDensity(0x8010);
}

// Direct3D light functions
D3DLIGHT8 gj_light[5];

void gjLoadLight_Directional(int i)
{
	// Initialize for directional light
	gj_light[i].Type = D3DLIGHT_DIRECTIONAL;

	gj_light[i].Diffuse.a = 1.0f;
	gj_light[i].Diffuse.r = 1.0f;
	gj_light[i].Diffuse.g = 1.0f;
	gj_light[i].Diffuse.b = 1.0f;

	gj_light[i].Specular.a = 0.0f;
	gj_light[i].Specular.r = 0.0f;
	gj_light[i].Specular.g = 0.0f;
	gj_light[i].Specular.b = 0.0f;

	gj_light[i].Ambient.a = 0.0f;
	gj_light[i].Ambient.r = 0.0f;
	gj_light[i].Ambient.g = 0.0f;
	gj_light[i].Ambient.b = 0.0f;

	gj_light[i].Direction.x = 1.0f;
	gj_light[i].Direction.y = 0.0f;
	gj_light[i].Direction.z = 0.0f;

	gj_light[i].Attenuation0 = 0.0f;
	gj_light[i].Attenuation1 = 0.0f;
	gj_light[i].Attenuation2 = 0.0f;

	gj_light[i].Falloff = 1.0f;
}

void gjLoadLight(int i)
{
	// Initialize for point light
	gj_light[i].Type = D3DLIGHT_POINT;

	gj_light[i].Diffuse.a = 1.0f;
	gj_light[i].Diffuse.r = 1.0f;
	gj_light[i].Diffuse.g = 1.0f;
	gj_light[i].Diffuse.b = 1.0f;

	gj_light[i].Specular.a = 0.0f;
	gj_light[i].Specular.r = 0.0f;
	gj_light[i].Specular.g = 0.0f;
	gj_light[i].Specular.b = 0.0f;

	gj_light[i].Ambient.a = 0.0f;
	gj_light[i].Ambient.r = 0.0f;
	gj_light[i].Ambient.g = 0.0f;
	gj_light[i].Ambient.b = 0.0f;

	gj_light[i].Position.x = 1.0f;
	gj_light[i].Position.y = 0.0f;
	gj_light[i].Position.z = 0.0f;

	gj_light[i].Range = 0.0f;
	gj_light[i].Falloff = 1.0f;

	gj_light[i].Attenuation0 = 0.0f;
	gj_light[i].Attenuation1 = 0.05f;
	gj_light[i].Attenuation2 = 0.0f;
}

void gjLightInit(void)
{
	// Enable lighting
	d3d_device->SetRenderState(D3DRS_LIGHTING, TRUE);
	d3d_device->SetRenderState(D3DRS_AMBIENT, 0xFF888888);

	// Setup lighting
	gjLoadLight_Directional(0);
	if ((gj_light[0].Type != D3DLIGHT_DIRECTIONAL) || (gj_light[0].Direction.z * gj_light[0].Direction.z + gj_light[0].Direction.y * gj_light[0].Direction.y + gj_light[0].Direction.x * gj_light[0].Direction.x) >= 0.03f)
	{
		d3d_device->SetLight(0, &gj_light[0]);
		d3d_device->LightEnable(0, TRUE);
	}
	gjLoadLight(1);
	if ((gj_light[1].Type != D3DLIGHT_DIRECTIONAL) || (gj_light[1].Direction.z * gj_light[1].Direction.z + gj_light[1].Direction.y * gj_light[1].Direction.y + gj_light[1].Direction.x * gj_light[1].Direction.x) >= 0.03f)
	{
		d3d_device->SetLight(1, &gj_light[1]);
		d3d_device->LightEnable(1, TRUE);
	}
	gjLoadLight(2);
	if ((gj_light[2].Type != D3DLIGHT_DIRECTIONAL) || (gj_light[2].Direction.z * gj_light[2].Direction.z + gj_light[2].Direction.y * gj_light[2].Direction.y + gj_light[2].Direction.x * gj_light[2].Direction.x) >= 0.03f)
	{
		d3d_device->SetLight(2, &gj_light[2]);
		d3d_device->LightEnable(2, TRUE);
	}
	gjLoadLight(3);
	if ((gj_light[3].Type != D3DLIGHT_DIRECTIONAL) || (gj_light[3].Direction.z * gj_light[3].Direction.z + gj_light[3].Direction.y * gj_light[3].Direction.y + gj_light[3].Direction.x * gj_light[3].Direction.x) >= 0.03f)
	{
		d3d_device->SetLight(3, &gj_light[3]);
		d3d_device->LightEnable(3, TRUE);
	}
	gjLoadLight(4);
	if ((gj_light[4].Type != D3DLIGHT_DIRECTIONAL) || (gj_light[4].Direction.z * gj_light[4].Direction.z + gj_light[4].Direction.y * gj_light[4].Direction.y + gj_light[4].Direction.x * gj_light[4].Direction.x) >= 0.03f)
	{
		d3d_device->SetLight(4, &gj_light[4]);
		d3d_device->LightEnable(4, TRUE);
	}
}

// Direct3D blend functions
void gjBlendInit(void)
{
	// Initialize render state for blending
	d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	d3d_device->SetRenderState(D3DRS_ALPHAREF, 0);
	d3d_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	d3d_device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, 0);
	d3d_device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, 0);
	d3d_device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, 0);
	d3d_device->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, 0);

	d3d_device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	d3d_device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
}

// Direct3D functions
void stSetDepthHigh(void)
{
	// Change depth format order
	d3d_depthstencil_format = d3d_depthstencil_format_high;
}

void stTryInitDevice(DWORD flags, D3DDEVTYPE device_type)
{
	if (d3d_device == NULL)
	{
		if (FAILED(d3d_d3d8->CreateDevice(d3d_adapter_id, device_type, d3d_present_parameters.hDeviceWindow, flags, &d3d_present_parameters, &d3d_device)))
			d3d_device = NULL;
	}
}

BOOL stInitDevice(const D3DFORMAT *format, const D3DFORMAT *depthstencil_format)
{
	D3DCAPS8 caps;

	// Setup present parameters
	memset(&d3d_present_parameters, 0, sizeof(d3d_present_parameters));
	d3d_present_parameters.BackBufferWidth = d3d_screen_width;
	d3d_present_parameters.BackBufferHeight = d3d_screen_height;
	d3d_present_parameters.Windowed = d3d_windowed;
	d3d_present_parameters.BackBufferFormat = *format;
	d3d_present_parameters.BackBufferCount = 1;
	d3d_present_parameters.EnableAutoDepthStencil = TRUE;
	d3d_present_parameters.AutoDepthStencilFormat = *depthstencil_format;
	d3d_present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3d_present_parameters.hDeviceWindow = main_hwnd;
	if (!d3d_windowed)
	{
		d3d_present_parameters.FullScreen_RefreshRateInHz = 0;
		d3d_present_parameters.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	// Create device
	if (d3d_device == NULL)
	{
		// Try to create HAL device
		if (d3d_d3d8->GetDeviceCaps(d3d_adapter_id, D3DDEVTYPE_HAL, &caps) == D3D_OK)
		{
			stTryInitDevice(D3DCREATE_HARDWARE_VERTEXPROCESSING, D3DDEVTYPE_HAL);
			stTryInitDevice(D3DCREATE_MIXED_VERTEXPROCESSING, D3DDEVTYPE_HAL);
			stTryInitDevice(D3DCREATE_SOFTWARE_VERTEXPROCESSING, D3DDEVTYPE_HAL);
		}
		if (d3d_device == NULL)
		{
			// Try to create reference device
			if (d3d_d3d8->GetDeviceCaps(d3d_adapter_id, D3DDEVTYPE_REF, &caps) == D3D_OK)
			{
				stTryInitDevice(D3DCREATE_HARDWARE_VERTEXPROCESSING, D3DDEVTYPE_REF);
				stTryInitDevice(D3DCREATE_MIXED_VERTEXPROCESSING, D3DDEVTYPE_REF);
				stTryInitDevice(D3DCREATE_SOFTWARE_VERTEXPROCESSING, D3DDEVTYPE_REF);
			}
			if (d3d_device == NULL)
				return FALSE;
		}
	}

	// Check for stencil buffer
	static const D3DFORMAT stencil_format[] = {D3DFMT_D24X4S4};
	for (int i = 0; i < sizeof(stencil_format) / sizeof(D3DFORMAT); i++)
		if (*depthstencil_format == stencil_format[i])
			st_d3d_flag |= 1;
	
	return TRUE;
}

void stFindDevice(void)
{
	// Get adapter count
	UINT adapter_count = d3d_d3d8->GetAdapterCount();
	if (d3d_adapter_id >= adapter_count)
		d3d_adapter_id = 0;

	D3DADAPTER_IDENTIFIER8 *adapter_id = new D3DADAPTER_IDENTIFIER8;
	d3d_d3d8->GetAdapterIdentifier(d3d_adapter_id, D3DENUM_NO_WHQL_LEVEL, adapter_id);
	delete adapter_id;

	// Iterate adapter modes for available formats
	UINT adapter_mode_count = d3d_d3d8->GetAdapterModeCount(d3d_adapter_id);

	int formati = 0;
	D3DFORMAT format[32];

	for (UINT i = 0; i < adapter_mode_count; i++)
	{
		// Get display mode
		D3DDISPLAYMODE display_mode;
		d3d_d3d8->EnumAdapterModes(d3d_adapter_id, i, &display_mode);

		// Check if it matches our requested mode
		if (display_mode.Width == d3d_screen_width && display_mode.Height == d3d_screen_height)
		{
			// Check format
			for (int j = 0; j < formati; j++)
			{
				if (format[formati] == display_mode.Format)
				{
					if (j != formati)
						continue;
					break;
				}
			}

			// Push format
			if (formati != (sizeof(format) / sizeof(D3DFORMAT)))
			{
				format[formati] = display_mode.Format;
				formati++;
			}
		}
	}

	if (formati != 0)
	{
		// Sort found formats by preference
		for (int i = 0, j = 0; i < d3d_pref_format_count; i++)
		{
			D3DFORMAT depthstencil_format = d3d_pref_format[i];
			for (; j < formati; j++)
			{
				for (int l = j; l < formati; l++)
				{
					if (format[j] == depthstencil_format)
					{
						format[l] = format[j];
						format[j] = depthstencil_format;
						j++;
						break;
					}
				}
			}
		}

		// Look for compatible device
		static const D3DDEVTYPE device_type[2] = {D3DDEVTYPE_HAL, D3DDEVTYPE_REF};

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < formati; j++)
			{
				if (d3d_d3d8->CheckDeviceType(d3d_adapter_id, device_type[i], format[j], format[j], d3d_windowed) == D3D_OK)
				{
					for (int k = 0; k < d3d_depthstencil_format_count; k++)
					{
						if (d3d_d3d8->CheckDeviceFormat(d3d_adapter_id, device_type[i], format[j], D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, d3d_depthstencil_format[k]) == D3D_OK)
						{
							if (stInitDevice(&format[j], &d3d_depthstencil_format[k]))
								return;
						}
					}
				}
			}
		}
	}
}

void stCreateDevice(void)
{
	// Check which adapter to use
	d3d_adapter_id = stTestCtrlFlag0(0x10) != 0;
	stFindDevice();
	if (d3d_device == NULL)
		neExit();
}

void stDirect3DInit(void)
{
	// Clear flag
	st_d3d_flag = 0;

	// Create Direct3D instance
	d3d_d3d8 = Direct3DCreate8(D3D_SDK_VERSION);
	if (d3d_d3d8 == NULL)
		neExit();

	// Create Direct3D device
	stCreateDevice();
	d3d_device->GetDeviceCaps(&d3d_caps);

	// Initialize Direct3D systems
	stFogInit();
	stTextureInit();
	mwPlyRestoreRsc();
	gjLightInit();
	gjBlendInit();
	gjTextureInit();
	mwPlyRestoreRsc();

	// Set buffer clear
	if (st_d3d_flag & 1)
		d3d_device->Clear(0, NULL, D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0, 1.0f, 0);
	else
		d3d_device->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0, 1.0f, 0);
	
	// Initialize view
	d3d_device->GetViewport(&d3d_viewport);
	d3d_viewcenterp[0] = (float)d3d_viewport.Width * 0.5f;
	d3d_viewcenterp[1] = (float)d3d_viewport.Height * 0.5f;

	d3d_viewrect[0] = (float)d3d_viewport.X;
	d3d_viewrect[1] = (float)d3d_viewport.Y;
	d3d_viewrect[2] = d3d_viewrect[0] + (float)(d3d_viewport.Width - 1);
	d3d_viewrect[3] = d3d_viewrect[1] + (float)(d3d_viewport.Height - 1);

	d3d_view = d3d_unitmatrix;

	// Check texture cubemap support
	if (!(d3d_caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP))
	{
		// TODO
	}
}

// System functions
int nj_display_mode;
int nj_frame_mode;
int nj_waitvsync_count;

int nj_frame_size_w;
int nj_frame_size_h;
NJS_SCREEN nj_screen;

void njInitSystem(Uint32 mode, Uint32 frame, Uint32 count)
{
	// Set globals
	nj_frame_mode = frame;
	nj_display_mode = mode;

	nj_waitvsync_count = count;
	//stHWSetVSync(count);

	// Get frame size
	if (mode & NJD_DSP_WIDTH_640)
	{
		nj_screen.w = 640.0f;
		nj_frame_size_w = 640;
	}
	else
	{
		nj_screen.w = 320.0f;
		nj_frame_size_w = 320;
	}
	if (mode & NJD_DSP_HEIGHT_480)
	{
		nj_screen.h = 480.0f;
		nj_frame_size_h = 480;
	}
	else
	{
		nj_screen.h = 240.0f;
		nj_frame_size_h = 240;
	}

	if (mode & NJD_DSP_ANTI)
	{
		nj_screen.w += nj_screen.w;
		if (nj_screen.h == 240.0f)
			nj_screen.h = 480.0f;
	}
}
