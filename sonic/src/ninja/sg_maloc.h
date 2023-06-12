#pragma once

#include <sg_xpt.h>

// Malloc functions
void syMallocInit(void *heap, Uint32 size);
void syMallocFinish(void);

void *syMalloc(Uint32 nbytes);
void *syCalloc(Uint32 nitems, Uint32 size);
void syFree(void *ap);
