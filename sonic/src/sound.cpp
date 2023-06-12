#include "sound.h"

// Sound globals
int sound_sound3d;

// Sound functions
void Sound_SetSound3D(int sound3d)
{
	if (sound3d > 3)
		sound3d = 3;
	if (sound3d < 0)
		sound3d = 0;
	sound_sound3d = sound3d;
}
