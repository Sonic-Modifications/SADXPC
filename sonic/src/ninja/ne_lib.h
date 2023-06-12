#pragma once

// Library structure
struct NES_LIB_PARAM
{
	LPTSTR Company;
	LPTSTR AppName;
	LPTSTR DVDRoot;
	LPTSTR BUPRoot;
	Bool Windowed;
	Uint32 GraphMode;
	Uint32 Frame;
	Uint32 Count;
	Float AspectX;
	Float AspectY;
	Uint32 PolyBuff_QUAD;
	Uint32 PolyBuff_CS;
	Uint32 PolyBuff_CS_N;
	Uint32 PolyBuff_CS_UVN;
	Uint32 PolyBuff_CS_UVN_N;
	Uint32 PolyBuff_CS_D8;
	Uint32 PolyBuff_CS_N_D8;
	Uint32 PolyBuff_CS_UVN_D8;
	Uint32 PolyBuff_CS_UVN_N_D8;
	Uint32 ZSearchMode;
	Uint32 Width;
	Uint32 Height;
};

// Library functions
extern HKEY ne_reg;
extern HKEY ne_reg_key_1, ne_reg_key_2;
void neRegOpenKey(void);
void neCtrlFlag(LPTSTR name, LPBYTE flag);

LPTSTR stQueryDVDRoot();
LPTSTR stQueryAppName();

void neInitLibrary(const NES_LIB_PARAM *lib);
void neExit();
