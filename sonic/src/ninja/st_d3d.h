#pragma once

#include <windows.h>
#include <d3d8.h>

// Direct3D globals
extern int d3d_windowed;
extern int d3d_screen_width;
extern int d3d_screen_height;

extern IDirect3D8 *d3d_d3d8;
extern IDirect3DDevice8 *d3d_device;

// Direct3D functions
void stWindowInit(void);
void stDirect3DInit(void);
