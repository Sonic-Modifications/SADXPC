#pragma once

#include <windows.h>

// Main globals
extern LPTSTR main_cmdline;
extern HINSTANCE main_hinst;
extern int main_showcmd;
extern HWND main_hwnd;
extern int main_result;

extern HANDLE main_mutex;

// Main functions
void DebugOut(const char *str, ...);
