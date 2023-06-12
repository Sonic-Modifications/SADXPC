#include "app.h"

#include "config.h"

#include "main.h"
#include "wmp.h"
#include "sound.h"
#include "input.h"

#include <ninja.h>
#include <sg_maloc.h>
#include <ne_lib.h>

// App constants
LPCTSTR app_ini = TEXT("..\\sonicDX.ini");
LPCTSTR app_ininame = TEXT("sonicDX");

LPCTSTR app_wmpchk_caption = TEXT("SonicAdventureDirectorsCut");
LPCTSTR app_wmpchk_text = TEXT("Windows Media Player 9 is not installed in your computer.\nSome of the sound will be silent.\nYou can install Windows Media Player 9 from Disc 1.");

LPCTSTR app_cdchk_caption = TEXT("SonicAdventureDirectorsCut");
LPCTSTR app_cdchk_text = TEXT("Insert Disk 2 of Sonic Adventure Deluxe in the CD drive and hit OK");

NES_LIB_PARAM app_lib = {
	TEXT("SONICTEAM"),
	TEXT("SonicAdventureDXPC"),
	TEXT(".\\SYSTEM"),
	TEXT("BUP\\"),
	1,
	NJD_DSP_VGA | NJD_DSP_640x480, NJD_FRAMEBUFFER_MODE_ARGB8888, 3,
	1.0f, 0.85f,
	8000, 4000, 8000, 12000, 24000,
	4000, 4000, 12000, 32000,
	0,
	640, 480,
};

// App globals
TCHAR app_cd[MAX_PATH];

int app_language;
int app_speech;

char app_fogemulation = 0;
int app_cliplevel;

int app_sevoice = 0;
int app_voicev;
int app_bgm;
int app_bgmv;

float app_screen_xscale = 1.0f;
float app_screen_yscale = 1.0f;

WORD app_pad[13];
int app_pad_map[18] = {1, 1, 0, 8, 9, 10, 11, 12, 5, 4, 3, -1, -1, -1, -1, -1};

// App functions
BOOL App_CheckDrive(LPTSTR drive)
{
	TCHAR name[MAX_PATH];
	HANDLE hfile;
	WIN32_FIND_DATA find_data;

	// Get file to check
	wsprintf(name, TEXT("%sSoundData\\SE\\V_AMY_J_BANK06.dat"), drive);

	// Find file
	hfile = FindFirstFile(name, &find_data);
	if (hfile == INVALID_HANDLE_VALUE)
	{
		// File wasn't found
		return FALSE;
	}

	// CD validated
	FindClose(hfile);
	return TRUE;
}

void App_FindCD(void)
{
	DWORD drive_len;
	LPTSTR drive_str;

	// Terminate string
	app_cd[0] = '\0';

	// Allocate buffer for drive string
	drive_len = GetLogicalDriveStrings(0, NULL);
	drive_str = (LPTSTR)syMalloc(drive_len * sizeof(TCHAR));

	if (drive_str != NULL)
	{
		// Read drive string
		if (GetLogicalDriveStrings(drive_len, drive_str) != 0)
		{
			// Iterate through drives
			int i = 0;
			if (drive_str[0] != '\0')
			{
				LPTSTR drive_v = drive_str;
				while (1)
				{
					if (GetDriveType(drive_v) != DRIVE_CDROM || !App_CheckDrive(drive_v))
					{
						// Iterate to next drive string
						if (drive_v[0] != '\0')
						{
							while (drive_str[++i]);
						}
						drive_v = &drive_str[++i];
						if (drive_str[i] == '\0')
							break;
					}
					else
					{
						// Valid CD
						lstrcpy(app_cd, &drive_str[i]);
						break;
					}
				}
			}
		}

		syFree(drive_str);
	}
}

#ifdef DEV_SYSTEM_DIR
#pragma comment(lib, "shlwapi.lib")
#include "shlwapi.h"
#endif

void App_Main(void)
{
	HMODULE hlib_ime;

#ifdef DEV_SYSTEM_DIR
	// Change system dir
	static TCHAR system_dir[MAX_PATH];
	GetModuleFileName(NULL, system_dir, MAX_PATH);
	PathRemoveFileSpec(system_dir); // Remove executable name
	PathRemoveFileSpec(system_dir); // Remove build directory
	lstrcat(system_dir, "\\SYSTEM");
	app_lib.DVDRoot = system_dir;
#endif

	// Disable IME
	if ((hlib_ime = LoadLibrary("imm32.dll")) != NULL)
	{
		BOOL (__stdcall *ImmDisableIME)(DWORD) = (BOOL (__stdcall*)(DWORD))GetProcAddress(hlib_ime, "ImmDisableIME");
		ImmDisableIME(0);
		DebugOut("immdisableime\n");
	}

	// Set language
	app_language = 1;
	app_speech = 1;
	DebugOut("Lamguage=%d\nSpeech=%d\n", app_language, app_speech);

	// Find CD drive
#ifndef NO_CD
	App_FindCD();
	while (app_cd[0] == '\0')
	{
		if (MessageBox(NULL, app_cdchk_text, app_cdchk_caption, MB_OKCANCEL) == IDCANCEL)
			goto Exit;
		App_FindCD();
	}
#else
	GetModuleFileName(NULL, app_cd, MAX_PATH);
#endif
	DebugOut("cddrive:%s\n", app_cd);

	// Check for Windows Media Player
	if (wmp_library.library == NULL || wmp_library.WMCreateReader == NULL)
	{
		MessageBox(NULL, app_wmpchk_text, app_wmpchk_caption, 0);
		DebugOut("media player can't found\n");
	}

	// Framerate
	app_lib.Count = GetPrivateProfileInt(app_ininame, TEXT("framerate"), 2, app_ini);
	if (app_lib.Count < 1)
		app_lib.Count = 1;
	if (app_lib.Count > 3)
		app_lib.Count = 3;

	DebugOut("framerate:%d\n", app_lib.Count);

	// Fog emulation
	int ini_fogemulation = GetPrivateProfileInt(app_ininame, TEXT("fogemulation"), 0, app_ini);
	if (ini_fogemulation < 0)
		ini_fogemulation = 0;
	if (ini_fogemulation > 1)
		ini_fogemulation = 1;

	if (ini_fogemulation)
		app_fogemulation |= ~1;
	else
		app_fogemulation &= ~1;

	DebugOut("fog:%d\n", ini_fogemulation);

	// Sound 3D
	int ini_sound3d = GetPrivateProfileInt(app_ininame, TEXT("sound3d"), 1, app_ini);
	if (ini_sound3d < 0)
		ini_sound3d = 0;
	if (ini_sound3d > 1)
		ini_sound3d = 1;

	Sound_SetSound3D(ini_sound3d);

	DebugOut("sound3d:%d\n", ini_sound3d);

	// Clip level
	app_cliplevel = GetPrivateProfileInt(app_ininame, TEXT("cliplevel"), 1, app_ini);
	if (app_cliplevel < 0)
		app_cliplevel = 0;
	if (app_cliplevel > 2)
		app_cliplevel = 2;

	DebugOut("cliplevel:%d\n", app_cliplevel);

	// SE Voice enable
	app_sevoice = GetPrivateProfileInt(app_ininame, TEXT("sevoice"), 1, app_ini);
	if (app_sevoice < 0)
		app_sevoice = 0;
	if (app_sevoice > 1)
		app_sevoice = 1;

	DebugOut("sevoice:%d\n", app_sevoice);

	// Voice volume
	int ini_voicev = GetPrivateProfileInt(app_ininame, TEXT("voicev"), 100, app_ini);
	if (ini_voicev <= 0)
	{
		ini_voicev = 0;
		app_sevoice = 0;
	}
	if (ini_voicev > 100)
		ini_voicev = 100;

	app_voicev = ((ini_voicev * 97) / 100 + -100) * 100;

	DebugOut("VOICE_VOLUME:%d\n", app_voicev);

	// BGM enable
	app_bgm = GetPrivateProfileInt(app_ininame, TEXT("bgm"), 1, app_ini);
	if (app_bgm < 0)
		app_bgm = 0;
	if (app_bgm > 1)
		app_bgm = 1;

	DebugOut("bgm:%d\n", app_bgm);

	// Voice volume
	int ini_bgmv = GetPrivateProfileInt(app_ininame, TEXT("bgmv"), 100, app_ini);
	if (ini_bgmv < 0)
		ini_bgmv = 0;
	if (ini_bgmv > 100)
		ini_bgmv = 100;

	app_bgmv = ((ini_bgmv * 89) / 100 + -100) * 100;

	DebugOut("BGM VOLUME:%d\n", app_bgmv);

	if (ini_bgmv == 0)
		ini_bgmv = -10000;

	// Screen mode
	int ini_screen = GetPrivateProfileInt(app_ininame, TEXT("screen"), 1, app_ini);
	if (ini_screen < 0)
		app_lib.Windowed = 0;
	if (ini_screen == 1)
		app_lib.Windowed = 1;
	if (ini_screen > 1)
	{
		app_lib.Windowed = 0;
		while (ShowCursor(FALSE) > 0);
	}

	DebugOut("screenmode:%d\n", ini_screen);

	// Screen size
	int ini_screensize = GetPrivateProfileInt(app_ininame, TEXT("screensize"), 0, app_ini);
	if (ini_screensize < 0 || ini_screensize > 4)
		ini_screensize = 0;

	switch (ini_screensize)
	{
		default:
			app_lib.Width = 640;
			app_lib.Height = 480;
			app_screen_xscale = 640.0f / 640.0f;
			app_screen_yscale = 480.0f / 480.0f;
			break;
		case 1:
			app_lib.Width = 800;
			app_lib.Height = 600;
			app_screen_xscale = 800.0f / 640.0f;
			app_screen_yscale = 600.0f / 480.0f;
			break;
		case 2:
			app_lib.Width = 1024;
			app_lib.Height = 768;
			app_screen_xscale = 1024.0f / 640.0f;
			app_screen_yscale = 768.0f / 480.0f;
			break;
		case 3:
			app_lib.Width = 1152;
			app_lib.Height = 864;
			app_screen_xscale = 1152.0f / 640.0f;
			app_screen_yscale = 864.0f / 480.0f;
			break;
		case 4:
			app_lib.Width = 1280;
			app_lib.Height = 1024;
			app_screen_xscale = 1280.0f / 640.0f;
			app_screen_yscale = 1024.0f / 480.0f;
			break;
	}

	// Pad buttons
	int ini_pad[9] = {2, 1, 13, 10, 9, 12, 17, 16, 3};

	TCHAR ini_padconfig[32];
	GetPrivateProfileString(app_ininame, TEXT("padconfig"), TEXT("pad_default"), ini_padconfig, sizeof(ini_padconfig) / sizeof(TCHAR), app_ini);

	int ini_buttons = GetPrivateProfileInt(ini_padconfig, TEXT("buttons"), 0, app_ini);
	if (ini_buttons)
	{
		if (ini_buttons > 13)
			ini_buttons = 13;
		memset(app_pad, 0xFF, sizeof(app_pad));
		app_pad[12] = -1;

		for (int i = 0; i < ini_buttons; i++)
		{
			TCHAR ini_padname[32];
			wsprintf(ini_padname, "pad%d", i);

			int ini_padv = GetPrivateProfileInt(ini_padconfig, ini_padname, -2, app_ini);
			if (ini_padv == -2)
			{
				if (i >= 9)
					continue;
				ini_padv = ini_pad[i];
			}

			if (ini_padv > 0 && ini_padv < 18)
			{
				int pad_map = app_pad_map[ini_padv];
				if (pad_map >= 0)
					app_pad[pad_map] = ini_padv;
			}
		}
	}

	DebugOut("padconfig done\n");

	// Mouse mode
	int ini_mousemode = GetPrivateProfileInt(app_ininame, TEXT("mousemode"), 0, app_ini);
	if (ini_mousemode < 0)
		ini_mousemode = 0;
	if (ini_mousemode > 1)
		ini_mousemode = 1;

	Input_SetMouseMode(ini_mousemode);

	// Mouse commands
	for (int i = 0; i < 5; i++)
	{
		TCHAR ini_cmdname[32];
		wsprintf(ini_cmdname, "cmd%d", i);

		int ini_cmdv = GetPrivateProfileInt(app_ininame, ini_cmdname, 0, app_ini);
		if (ini_cmdv > 0)
			Input_SetMouseCmd(i, ini_cmdv);
	}

	DebugOut("mouseconfig done\n");

	// Initialize COM
	CoInitialize(NULL);
	DebugOut("COM Initialize\n");

	// Initialize Ninja Library
	neInitLibrary(&app_lib);
	DebugOut("Sonic Library Initialize\n");

	// Deinitialize COM
	CoUninitialize();
	DebugOut("COM Uninitialize\n");

Exit:
	// Release IME library
	if (hlib_ime != NULL)
	{
		FreeLibrary(hlib_ime);
		DebugOut("Free imm32\n");
	}
	DebugOut("All Finalize done\n");
}
