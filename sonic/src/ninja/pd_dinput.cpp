#include <pd_dinput.h>

#include "main.h"

#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include <ninja/ninjaapi.h>
#include "app.h"

// Libraries
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

// DirectInput globals
PDS_PERIPHERALINFO PeripheralInfo[4];
PDS_PERIPHERAL PeripheralData[4];

PDS_PERIPHERAL *di_peripheral_data;
int di_peripheral_count;

IDirectInput8 *di_dinput8;

// DirectInput device functions
struct diDEVICE
{
	DIDEVCAPS caps;
	IDirectInputDevice8 *device;
	IDirectInputEffect *effect;
	DIEFFECT effect_def;
	LONG xaxis_min;
	LONG xaxis_max;
	LONG yaxis_min;
	LONG yaxis_max;
	LONG raxis1_min;
	LONG raxis1_max;
	LONG raxis2_min;
	LONG raxis2_max;
	DWORD raxis1;
	DWORD raxis2;
	DWORD effect_axes;
} di_device[4];
int di_devicei;

BOOL PASCAL pdEnumAxes(LPCDIDEVICEOBJECTINSTANCE lp_ddoi, LPVOID pv_ref)
{
	IDirectInputDevice8 *device = (IDirectInputDevice8*)pv_ref;
	diDEVICE *lp_device = &di_device[di_devicei];

	// Get object range
	DIPROPRANGE prop_range;
	prop_range.diph.dwObj = lp_ddoi->dwType;
	prop_range.diph.dwSize = sizeof(DIPROPRANGE);
	prop_range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	prop_range.diph.dwHow = DIPH_BYID;
	if (device->GetProperty(DIPROP_RANGE, &prop_range.diph) != DI_OK)
		return FALSE;

	// Setup axes
	if (lp_ddoi->guidType == GUID_XAxis)
	{
		lp_device->xaxis_min = prop_range.lMin;
		lp_device->xaxis_max = prop_range.lMax;
	}
	if (lp_ddoi->guidType == GUID_YAxis)
	{
		lp_device->yaxis_min = prop_range.lMin;
		lp_device->yaxis_max = prop_range.lMax;
	}
	if (lp_ddoi->guidType == GUID_RxAxis)
	{
		lp_device->raxis2_min = prop_range.lMin;
		lp_device->raxis2_max = prop_range.lMax;
	}
	if (lp_ddoi->guidType == GUID_RyAxis)
	{
		lp_device->raxis1_min = prop_range.lMin;
		lp_device->raxis1_max = prop_range.lMax;
	}
	if (lp_ddoi->guidType == GUID_RzAxis)
	{
		if (lp_device->raxis2 == -1)
		{
			lp_device->raxis2_min = prop_range.lMin;
			lp_device->raxis2_max = prop_range.lMax;
			lp_device->raxis2 = 2;
		}
		else if (lp_device->raxis1 == -1)
		{
			lp_device->raxis1_min = prop_range.lMin;
			lp_device->raxis1_max = prop_range.lMax;
			lp_device->raxis1 = 2;
		}
	}
	if (lp_ddoi->guidType == GUID_Slider)
	{
		if (lp_device->raxis2 == -1)
		{
			lp_device->raxis2_max = prop_range.lMax;
			lp_device->raxis2_min = prop_range.lMin;
			lp_device->raxis2 = 3;
		}
		if (lp_device->raxis1 == -1)
		{
			lp_device->raxis1_min = prop_range.lMin;
			lp_device->raxis1_max = prop_range.lMax;
			lp_device->raxis1 = 3;
		}
	}

	return TRUE;
}

int di_device_actcount;
BOOL PASCAL pdEnumActuator(LPCDIDEVICEOBJECTINSTANCE lp_ddoi, LPVOID pv_ref)
{
	// Count actuators
	if (lp_ddoi->dwFlags & DIDOI_FFACTUATOR)
		di_device_actcount++;
	return TRUE;
}

BOOL PASCAL pdEnumDevice(LPCDIDEVICEINSTANCE lp_ddi, LPVOID pv_ref)
{
	// Create device
	IDirectInputDevice8 *device;
	if (di_dinput8->CreateDevice(lp_ddi->guidInstance, &device, NULL) == DI_OK)
	{
		// Setup device
		diDEVICE *lp_device = &di_device[di_devicei];
		lp_device->device = device;

		device->SetDataFormat(&c_dfDIJoystick2);
		device->SetCooperativeLevel(main_hwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND);
		device->GetCapabilities(&lp_device->caps);

		// Find axes
		device->EnumObjects(pdEnumAxes, device, DIDFT_AXIS);

		// Disable axis auto center
		DIPROPDWORD prop_autocenter;
		prop_autocenter.diph.dwSize = sizeof(DIPROPDWORD);
		prop_autocenter.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		prop_autocenter.diph.dwObj = 0;
		prop_autocenter.diph.dwHow = DIPH_DEVICE;
		prop_autocenter.dwData = DIPROPAUTOCENTER_OFF;
		device->SetProperty(DIPROP_AUTOCENTER, &prop_autocenter.diph);

		// Find actuators
		DWORD effect_axes[2] = {0, 4};
		LONG effect_dir[2] = {0, 0};
		DICONSTANTFORCE effect_param = {0};

		di_device_actcount = 0;
		device->EnumObjects(pdEnumActuator, NULL, DIDFT_AXIS);
		if (di_device_actcount > 2)
			di_device_actcount = 2;
		if (di_device_actcount > 0)
		{
			// Setup effect
			lp_device->effect_def.dwDuration = -1;
			lp_device->effect_def.dwTriggerButton = -1;
			lp_device->effect_axes = di_device_actcount;
			lp_device->effect_def.cAxes = di_device_actcount;
			lp_device->effect_def.rgdwAxes = effect_axes; // Surely it's unsafe to put a stack pointer here
			lp_device->effect_def.rglDirection = effect_dir; // ;
			lp_device->effect_def.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
			lp_device->effect_def.dwSamplePeriod = 0;
			lp_device->effect_def.dwGain = 10000;
			lp_device->effect_def.dwTriggerRepeatInterval = 0;
			lp_device->effect_def.lpEnvelope = NULL;
			lp_device->effect_def.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
			lp_device->effect_def.lpvTypeSpecificParams = &effect_param;
			lp_device->effect_def.dwStartDelay = 0;
			lp_device->effect_def.dwSize = sizeof(DIEFFECT);

			// Create effect
			device->CreateEffect(GUID_ConstantForce, &lp_device->effect_def, &lp_device->effect, NULL);
		}
		
		// Increment device index
		di_devicei++;
	}

	return TRUE;
}

void pdDirectInputUpdate(void)
{
	for (int i = 0; di_peripheral_count; i++)
	{
		PDS_PERIPHERAL *lp_peripheral = &di_peripheral_data[i];
		diDEVICE *lp_device = &di_device[i];
		IDirectInputDevice8 *device = lp_device->device;

		// Reset peripheral button state
		Uint32 old_on = lp_peripheral->on;
		lp_peripheral->on = 0;
		lp_peripheral->off = 0;
		lp_peripheral->press = 0;
		lp_peripheral->release = 0;

		if (device != NULL)
		{
			// Poll device
			switch (device->Poll())
			{
				case DIERR_INPUTLOST:
				case DIERR_NOTACQUIRED:
				{
					while (device->Acquire() == DIERR_INPUTLOST);
					break;
				}
				case 0:
				case 1:
				{
					DIJOYSTATE2 joystate;
					if (device->GetDeviceState(sizeof(DIJOYSTATE2), (LPVOID)&joystate) == DI_OK)
					{
						// Get dpad state from POV hat
						if (joystate.rgdwPOV[0] != -1)
						{
							ULONG hat_deg = joystate.rgdwPOV[0] / 100;
							if (hat_deg < 360)
							{
								if (hat_deg >= 315 || hat_deg < 45)
									lp_peripheral->on |= PDD_DGT_KU;
								if (hat_deg >= 45 && hat_deg < 135)
									lp_peripheral->on |= PDD_DGT_KR;
								if (hat_deg >= 135 && hat_deg < 225)
									lp_peripheral->on |= PDD_DGT_KD;
								if (hat_deg >= 225 && hat_deg < 315)
									lp_peripheral->on |= PDD_DGT_KL;
							}
						}

						// Get mapped button state
						struct
						{
							DWORD i;
							Uint32 bit;
						} button_state[14] = {
							{ 8, PDD_DGT_ST},
							{ 8, PDD_DGT_ST},
							{ 9, PDD_DGT_KU},
							{10, PDD_DGT_KD},
							{11, PDD_DGT_KL},
							{12, PDD_DGT_KR},
							{ 0, PDD_DGT_TA},
							{ 1, PDD_DGT_TB},
							{ 2, PDD_DGT_TC},
							{ 3, PDD_DGT_TX},
							{ 4, PDD_DGT_TY},
							{ 5, PDD_DGT_TZ},
							{ 6, PDD_DGT_TL},
							{ 7, PDD_DGT_TR},
						};

						for (int j = 0; j < 14; j++)
						{
							if (joystate.rgbButtons[app_pad[button_state[j].i]] & 0x80)
								lp_peripheral->on |= button_state[j].bit;
						}

						// Get analogue stick state
						int stick_x = ((joystate.lX - lp_device->xaxis_min) * 0xFF) / (lp_device->xaxis_max - lp_device->xaxis_min) - 0x7F;
						int stick_xm = 1;
						if (stick_x <= 0)
						{
							stick_xm = -1;
							stick_x = -stick_x;
						}

						short stick_sx;
						if (stick_x < 0x10)
							stick_sx = 0;
						else
							stick_sx = ((stick_x - 0x10) << 7) / 0x80;
						
						lp_peripheral->x1 = stick_sx * stick_xm;

						int stick_y = ((joystate.lY - lp_device->yaxis_min) * 0xFF) / (lp_device->yaxis_max - lp_device->yaxis_min) - 0x7F;
						int stick_ym = 1;
						if (stick_y <= 0)
						{
							stick_ym = -1;
							stick_y = -stick_y;
						}

						short stick_sy;
						if (stick_y < 0x10)
							stick_sy = 0;
						else
							stick_sy = ((stick_y - 0x10) << 7) / 0x80;

						lp_peripheral->y1 = stick_sy * stick_ym;

						// Get rotation axes state
						LONG raxis;
						switch (lp_device->raxis2)
						{
							case 0:
								raxis = joystate.lRx;
								break;
							case 1:
								raxis = joystate.lRy;
								break;
							case 2:
								raxis = joystate.lRz;
								break;
							case 3:
								raxis = joystate.rglSlider[0];
								break;
							case 4:
								raxis = joystate.rglSlider[1];
								break;
							default:
								raxis = 0;
								break;
						}

						int stick_r = ((raxis - lp_device->raxis2_min) * 0xFF) / (lp_device->raxis2_max - lp_device->raxis2_min) - 0x7F;
						int stick_rm = 1;
						if (stick_r <= 0)
						{
							stick_rm = -1;
							stick_r = -stick_r;
						}

						short stick_sr;
						if (stick_r < 0x10)
							stick_sr = 0;
						else
							stick_sr = ((stick_r - 0x10) << 7) / 0x80;

						lp_peripheral->x2 = stick_sr * stick_rm;
					}
					break;
				}
			}
		}
	}
}

// DirectInput vibration functions
// TODO
struct diVIB
{
	int mode;
	int unk1;
	int unk2;
	int unk3;
	int unk4;
	int unk5;
} di_vib[4];

void pdVibMxServer(void)
{
	for (int i = 0; i < 4; i++)
	{
		switch (di_vib[i].mode)
		{
			case 1:
			case 2:
				break;
			case 3:
				break;
		}
	}
}

void pdVibMxInit(void)
{
	// Initialize vibration state
	for (int i = 0; i < 4; i++)
		di_vib[i].mode = 0;
	njEntryWaitVSyncFunc(pdVibMxServer);
}


// DirectInput functions
void pdDirectInputInit(PDS_PERIPHERAL *peripheral_data, int count)
{
	// Set DirectInput peripheral globals
	di_peripheral_data = peripheral_data;
	di_peripheral_count = count;

	// Initialize devices
	for (int i = 0; i < 4; i++)
	{
		di_device[i].device = NULL;
		di_device[i].caps.dwSize = sizeof(DIDEVCAPS);
		di_device[i].effect = NULL;
		di_device[i].xaxis_min = -128;
		di_device[i].xaxis_max =  127;
		di_device[i].yaxis_min = -128;
		di_device[i].yaxis_max =  127;
		di_device[i].raxis1_min = -128;
		di_device[i].raxis1_max =  127;
		di_device[i].raxis2_min = -128;
		di_device[i].raxis2_max =  127;
		di_device[i].raxis1 = -1;
		di_device[i].raxis2 = -1;
		di_device[i].effect_axes = 0;
	}

	// Initialize DirectInput
	if (di_dinput8 == NULL)
		DirectInput8Create(main_hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&di_dinput8, NULL);

	// Enumerate connected devices
	di_devicei = 0;
	di_dinput8->EnumDevices(DI8DEVCLASS_GAMECTRL, pdEnumDevice, NULL, DIEDFL_ATTACHEDONLY);

	// Connect peripherals as controllers
	for (int i = 0; i < count; i++)
		peripheral_data[i].info->type |= PDD_DEVTYPE_CONTROLLER;
}

void pdInitPeripheral(Uint32 logic, void *recvbuf, void *sendbuf)
{
	// Initialize peripheral infos
	for (int i = 0; i < 4; i++)
		memset(&PeripheralInfo[i], 0, sizeof(PDS_PERIPHERALINFO));

	// Initialize peripherals
	for (int i = 0; i < 4; i++)
	{
		memset(&PeripheralData[i], 0, sizeof(PDS_PERIPHERAL));
		PeripheralData[i].info = &PeripheralInfo[i];
	}

	// Initialize DirectInput
	pdDirectInputInit(PeripheralData, 4);
	njEntryWaitVSyncFunc(pdDirectInputUpdate);

	// Initialize vibration
	pdVibMxInit();
}
