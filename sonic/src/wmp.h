#pragma once

#include <windows.h>

// Windows Media Player library
class WMP_Library
{
	public:
		// Library and WMCreateReader
		HMODULE library;
		FARPROC WMCreateReader;

	public:
		WMP_Library()
		{
			// Load library and get WMCreateReader address
			if ((library = LoadLibrary(TEXT("WMVCORE.DLL"))) != 0)
				WMCreateReader = GetProcAddress(library, "WMCreateReader");
			else
				WMCreateReader = NULL;
		}

		~WMP_Library()
		{
			// Release library
			if (library != NULL)
				FreeLibrary(library);
			library = NULL;
		}
};
extern WMP_Library wmp_library;
