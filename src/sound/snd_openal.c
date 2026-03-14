//////////////////////////////////////////////////////////////////////////
// OpenAL

#include "../stdafx.h"
#include "snd_openal.h"

static snd_device_t* sounddevice;

//
// OpenAL intialisieren (http://www.devmaster.net/articles/openal/)
//
ALboolean sndALInitSound()
{
	sounddevice = (snd_device_t*)malloc(sizeof(snd_device_t));
	if(sounddevice == NULL)
		return AL_FALSE;

	sounddevice->device = alcOpenDevice((const ALCchar*)"DirectSound3D");
	if(sounddevice->device == NULL)
	{
		free(sounddevice);
		return AL_FALSE;
	}

	return AL_TRUE;
}

//
// Aufr�umen und benutzte Ressourcen freigeben
//
ALboolean sndALCleanUp()
{
	ALCcontext* context;
	ALCdevice* device;

	//Get active context
	context = alcGetCurrentContext();
	//Get device for active context
	device = alcGetContextsDevice(context);
	//Disable context
	alcMakeContextCurrent(NULL);
	//Release context(s)
	alcDestroyContext(context);
	//Close device
	alcCloseDevice(device);

	return AL_TRUE;
}
