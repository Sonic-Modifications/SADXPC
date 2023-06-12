#pragma once

#include <ninjadef.h>

// Ninja structures
typedef struct {
	NJS_MATRIX m;
	Float   px,py,pz;
	Float   vx,vy,vz;
	Angle   roll;
	Float   apx,apy,apz;
	Float   avx,avy,avz;
	Angle   aroll;
} NJS_VIEW;

struct NJS_SCREEN
{
	Float dist;
	Float w, h;
	Float cx, cy;
};
