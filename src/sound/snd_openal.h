//////////////////////////////////////////////////////////////////////////
// Soundunterst�tzung

#ifndef _SND_OPENAL_H
#define _SND_OPENAL_H

// OpenAL Support (ben�tigt openAL SDK www.openal.org)
#pragma comment (lib, "openal32.lib")

#include <AL/al.h>
#include <AL/alc.h>

typedef struct {
	ALCcontext *context;
	ALCdevice  *device;
} snd_device_t;

ALboolean sndALInitSound();
ALboolean sndALCleanUp();

#endif _SND_OPENAL_H


