#include <ninja.h>
#include <sg_sytmr.h>

// Timer globals
int timer_flag;
LARGE_INTEGER timer_base;
LARGE_INTEGER timer_stop;
LARGE_INTEGER timer_freq;
void (*timer_int_func)(void);
int timer_int_arg;

// Timer functions
void syTmrInit(void)
{
	// Initialize timer
	timer_flag = 0;
	timer_base.QuadPart = 0;
	timer_stop.QuadPart = 0;
	QueryPerformanceFrequency(&timer_freq);
	timer_int_func = NULL;
	timer_int_arg = 0;
}

unsigned long syTmrGetCount(void)
{
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	return (unsigned long)(1000000 * count.QuadPart / timer_freq.QuadPart);
}
