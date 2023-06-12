#include "input.h"

// Input globals
int input_mousemode;

int input_mousecmd_i;
struct
{
	short cmd;
	unsigned short i;
} input_mousecmd[32];

// Input functions
void Input_SetMouseMode(int mousemode)
{
	input_mousemode = mousemode;
}

void Input_SetMouseCmd(unsigned short i, unsigned short cmd)
{
	static short cmds[10] = {8, 0, 2, 0, 4, 0, 1024, 0, 512, 0};
	while (input_mousecmd_i < 32)
	{
		input_mousecmd[input_mousecmd_i].cmd = cmd;
		input_mousecmd[input_mousecmd_i].i = cmds[i];
		input_mousecmd_i++;
		if (cmd < 3 || cmd >= 256)
			return;
		cmd |= 0x100;
	}
}
