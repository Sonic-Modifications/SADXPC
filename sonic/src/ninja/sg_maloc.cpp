#include <sg_maloc.h>

// Malloc constants
#define HEAP_SIZE 0x800000

// Malloc globals
static HANDLE malloc_heap = INVALID_HANDLE_VALUE;

// Malloc functions
void syMallocInit(void *heap, Uint32 size)
{
	if (malloc_heap == INVALID_HANDLE_VALUE)
		malloc_heap = HeapCreate(0, HEAP_SIZE, 0);
}

void syMallocFinish(void)
{
	HeapDestroy(malloc_heap);
	malloc_heap = INVALID_HANDLE_VALUE;
}

void *syMalloc(Uint32 nbytes)
{
	if (malloc_heap == INVALID_HANDLE_VALUE)
		malloc_heap = HeapCreate(0, HEAP_SIZE, 0);
	return HeapAlloc(malloc_heap, 0, nbytes);
}

void *syCalloc(Uint32 nitems, Uint32 size)
{
	// Does not ensure malloc heap exists
	return HeapAlloc(malloc_heap, HEAP_ZERO_MEMORY, nitems * size);
}

void syFree(void *ap)
{
	if (ap != NULL)
		HeapFree(malloc_heap, 0, ap);
}
