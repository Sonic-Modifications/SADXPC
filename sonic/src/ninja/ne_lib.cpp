#include <ninja.h>
#include <ne_lib.h>

#include <sg_maloc.h>
#include <sg_syhw.h>
#include <sg_pad.h>
#include <sg_syrtc.h>

#include "ninja/st_d3d.h"

// Ninja Library globals
TCHAR lib_company[64] = TEXT("SonicTeam");
TCHAR lib_appname[64] = TEXT("PSOV2");
TCHAR lib_dvdroot[MAX_PATH];
TCHAR lib_buproot[MAX_PATH];

int lib_PolyBuff_QUAD = 8000;
int lib_PolyBuff_CS = 4000;
int lib_PolyBuff_CS_N = 8000;
int lib_PolyBuff_CS_UVN = 12000;
int lib_PolyBuff_CS_UVN_N = 24000;
int lib_PolyBuff_CS_D8 = 4000;
int lib_PolyBuff_CS_N_D8 = 4000;
int lib_PolyBuff_CS_UVN_D8 = 12000;
int lib_PolyBuff_CS_UVN_N_D8 = 32000;

char lib_pad_recv[0xC000];
char lib_pad_send[0xC000];

// Ninja Library functions
HKEY ne_reg;
HKEY ne_reg_key_1, ne_reg_key_2;

void neRegOpenKey(void)
{
	RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software"), 0, KEY_READ, &ne_reg);
	RegOpenKeyEx(ne_reg, lib_company, 0, KEY_READ, &ne_reg_key_1);
	RegOpenKeyEx(ne_reg_key_1, lib_appname, 0, KEY_READ, &ne_reg_key_2);
}

void neCtrlFlag(LPTSTR name, LPBYTE flag)
{
	DWORD data;
	DWORD v3;

	neRegOpenKey();
	RegQueryValueEx(ne_reg_key_2, name, 0, 0, 0, &data);
	RegCloseKey(ne_reg_key_2);
	RegCloseKey(ne_reg_key_1);
	RegCloseKey(ne_reg);

	*flag = (BYTE)data;

	neRegOpenKey();
	RegQueryValueEx(ne_reg_key_2, name, 0, 0, flag, &v3);
	RegCloseKey(ne_reg_key_2);
	RegCloseKey(ne_reg_key_1);
	RegCloseKey(ne_reg);
}

LPTSTR stQueryDVDRoot()
{
	return lib_dvdroot;
}

LPTSTR stQueryAppName()
{
	return lib_appname;
}

void stQueryInit(char **names)
{
	strcpy(lib_company, names[0]);
	strcpy(lib_appname, names[1]);
	strcpy(lib_dvdroot, names[2]);
	strcpy(lib_buproot, names[3]);
}

void neInitLibrary(const NES_LIB_PARAM *lib)
{
	// Set library globals
	char *lib_names[] = {
		lib->Company,
		lib->AppName,
		lib->DVDRoot,
		lib->BUPRoot
	};

	lib_PolyBuff_QUAD = lib->PolyBuff_QUAD;
	lib_PolyBuff_CS = lib->PolyBuff_CS;
	lib_PolyBuff_CS_N = lib->PolyBuff_CS_N;
	lib_PolyBuff_CS_UVN = lib->PolyBuff_CS_UVN;
	lib_PolyBuff_CS_UVN_N = lib->PolyBuff_CS_UVN_N;
	lib_PolyBuff_CS_D8 = lib->PolyBuff_CS_D8;
	lib_PolyBuff_CS_N_D8 = lib->PolyBuff_CS_N_D8;
	lib_PolyBuff_CS_UVN_D8 = lib->PolyBuff_CS_UVN_D8;
	lib_PolyBuff_CS_UVN_N_D8 = lib->PolyBuff_CS_UVN_N_D8;

	d3d_windowed = lib->Windowed;
	d3d_screen_width = lib->Width;
	d3d_screen_height = lib->Height;
	if (lib->ZSearchMode == 1)
		stSetDepthHigh();

	stQueryInit(lib_names);

	// Initialize system
	stWindowInit();
	mwPlyRestoreRsc();
	syHwInit();
	syMallocInit(NULL, 0);
	njInitSystem(lib->GraphMode, lib->Frame, lib->Count);
	mwPlyRestoreRsc();
	pdInitPeripheral(PDD_PLOGIC_ACTIVE, lib_pad_recv, lib_pad_send);
	syRtcInit();
}

void neExit()
{
	exit(0);
}
