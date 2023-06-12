#pragma once

#include <ninjadef.h>

// Ninja functions
void njInitSystem(Uint32 mode, Uint32 frame, Uint32 count);

void njEntryWaitVSyncFunc(void (*func)(void));
void njDeleteWaitVSyncFunc(void (*func)(void));
void njExecWaitVsyncFunc(void);
void njInitWaitVsyncFunc(void);
