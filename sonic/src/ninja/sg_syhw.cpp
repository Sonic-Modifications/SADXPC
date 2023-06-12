#include <ninja.h>
#include <sg_syhw.h>

#include <sg_sytmr.h>

#include <ninja/st_d3d.h>

// Hardware functions
void mwPlyRestoreRsc(void) {;}

void syHwInit()
{
	stDirect3DInit();
	syTmrInit();
}
