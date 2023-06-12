#pragma once

#include <st_d3d.h>

// Direct3D texture globals
struct stTEXFUNCS
{
	void *func0;
	int field1_0x4;
	int field2_0x8;
	int field3_0xc;
	void *func1;
	int field5_0x14;
	int field6_0x18;
	int field7_0x1c;
	void *func2;
	int field9_0x24;
	int field10_0x28;
	int field11_0x2c;
};

struct stTEXFUNCC
{
	stTEXFUNCS *funcs0;
	stTEXFUNCS *funcs1;
	void *field2_0x8;
	void *field3_0xc;
};

extern stTEXFUNCC st_texfuncs[4];

// Direct3D texture functions
void stTextureInit(void);
void gjTextureInit(void);
