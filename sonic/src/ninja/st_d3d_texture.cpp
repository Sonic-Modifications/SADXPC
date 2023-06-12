#include <st_d3d_texture.h>

// Direct3D texture globals
int st_texture_flag;
int st_texture_last_globalindex;
int st_texture_last_bankid;
int st_texture_loadpalette;

struct
{
	int unk0;
	int unk1;
	int unk2;
	int unk3;
} st_texture_palette[128];
struct
{
	unsigned char unk0, unk1, unk2, unk3;
} st_texture_palette_color[512];

// Texfuncs
stTEXFUNCC st_texfuncs[4];

// Direct3D texture functions
void stTextureInit(void)
{
	// Initialize texture state
	st_texture_flag = 0;
	st_texture_last_globalindex = -1;
	st_texture_last_bankid = -1;
	st_texture_loadpalette = 0;

	// Initialize palette
	for (int i = 0; i < 128; i++)
	{
		st_texture_palette[i].unk1 = i;
		st_texture_palette[i].unk0 = -1;
		st_texture_palette[i].unk2 = 0;
	}
	for (int i = 0; i < 512; i++)
	{
		st_texture_palette_color[i].unk0 = 0;
		st_texture_palette_color[i].unk1 = 0;
		st_texture_palette_color[i].unk2 = 0;
		st_texture_palette_color[i].unk3 = 0;
	}
}

void gjTextureInit(void)
{
	// Set device texture stages
	d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);

	d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	d3d_device->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	d3d_device->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	d3d_device->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
}
