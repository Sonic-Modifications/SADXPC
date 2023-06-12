#include <ninja.h>

// Ninja globals
int nj_system_vsync_func_num;
void (*nj_system_vsync_func_tbl[32])(void);

void (*nj_eor_function)(void);
void (*nj_eor_function2)(void);
void (*nj_system_vsync_function)(void);
void (*nj_system_waitvsync_function)(long);
long nj_system_waitvsync_function_arg;

// Ninja functions
void njEntryWaitVSyncFunc(void (*func)(void))
{
	// Push function onto table
	nj_system_vsync_func_tbl[nj_system_vsync_func_num++] = func;
}

void njDeleteWaitVSyncFunc(void (*func)(void))
{
	if (nj_system_vsync_func_num != 0)
	{
		// Look for function in table
		int i = 0;
		while (1)
		{
			if (nj_system_vsync_func_tbl[i] == func)
			{
				nj_system_vsync_func_tbl[i] = NULL;
				if (nj_system_vsync_func_num != 1)
					break;
			}
			if (++i == nj_system_vsync_func_num)
				return;
		}

		// Pop function from table
		nj_system_vsync_func_num--;
		nj_system_vsync_func_tbl[nj_system_vsync_func_num] = nj_system_vsync_func_tbl[nj_system_vsync_func_num + 1];
	}
}

void njExecWaitVsyncFunc(void)
{
	// Execute function table
	for (int i = 0; i < nj_system_vsync_func_num; i++)
		nj_system_vsync_func_tbl[i]();

	// Execute built-in functions
	if (nj_eor_function != NULL)
		nj_eor_function();
	if (nj_eor_function2 != NULL)
		nj_eor_function2();
	if (nj_system_vsync_function != NULL)
		nj_system_vsync_function();
	if (nj_system_waitvsync_function != NULL)
		nj_system_waitvsync_function(nj_system_waitvsync_function_arg);
}

void njInitWaitVsyncFunc(void)
{
	// Initialize vsync function state
	nj_system_vsync_func_num = 0;
}
