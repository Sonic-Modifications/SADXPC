#include "main.h"

#include "config.h"
#include "app.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

// Main constants
LPCSTR main_lpname = TEXT("NINJA for PC");

// Main globals
LPTSTR main_cmdline;
HINSTANCE main_hinst;
int main_showcmd;
HWND main_hwnd;
int main_result;

HANDLE main_mutex;

// Debug out, stubbed in release
void DebugOut(const char *str, ...)
{
#ifdef ENABLE_DEBUG_OUT
	static char debugstr[512];

	va_list args;
	va_start(args, str);
	vsprintf(debugstr, str, args);
	va_end(args);

	OutputDebugStringA(debugstr);
	fprintf(stdout, "%s", debugstr);
#endif
}

// Entry point
#ifndef UNICODE // _tWinMain doesn't work in VS2003
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
#endif
{
	SECURITY_ATTRIBUTES mutex_attr;

	// Set main globals
	main_cmdline = lpCmdLine;
	main_hinst = hInstance;
	main_showcmd = nShowCmd;
	main_hwnd = NULL;
	main_result = -1;

	// Create mutex
	mutex_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
	mutex_attr.lpSecurityDescriptor = NULL;
	mutex_attr.bInheritHandle = TRUE;

	main_mutex = CreateMutex(&mutex_attr, FALSE, main_lpname);
	if (GetLastError() != ERROR_ALREADY_EXISTS)
		App_Main();

	return main_result;
}
