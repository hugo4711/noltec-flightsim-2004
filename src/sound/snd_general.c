//////////////////////////////////////////////////////////////////////////
// Generelle Sound-Api Funktionen

#include "../stdafx.h"
#include "snd_general.h"

static int sound_api_flags = NULL;	// Flags die die Fähigkeiten der Sound-Api betreffen

//
// Sound initialisieren. Prüfen welche Sound-Api unterstützt wird und entsprechende Flags zurückgeben
// Anhand dieser Flags kann dann entschieden werden, mit welcher Api die Sounds geladen und
// abgespielt werden
int sndGeneralSoundInit()
{

	//int sound_api_flags;

	//////////////////////////////////////////////////////////////////////////
	// Sound initialisieren
	// Wenn OpenAL unterstützung nicht vorhanden, dann DirectSound nutzen
	//////////////////////////////////////////////////////////////////////////


	//++++++++++++ OpenAL +++++++++++++++++

	devconout("initializing sound (OpenAL) ... ");
	if(sndALInitSound() == AL_TRUE)
	{
		sound_api_flags |= OPENAL_OUTPUT; // Anhand dieser Variable kann entschieden werden welcher
		// Soundtyp unterstützt wird.
		devconout("o.k.\n");
	}
	else
	{
		devconout("not supported.\n");
	}

	//++++++++++++ DirectSound +++++++++++++

	devconout("initializing sound (DirectSound9) ... ");
	if(sndDX9InitSound3D() == TRUE)
	{
		sound_api_flags |= DX9SND_OUTPUT;
		devconout("o.k.\n");


	}
	else
		devconout("not supported.\n");

	// ++++++++++++++++ Spezialfall. ++++++++++++++++++++++
	// Falls OpenAL UND DirectSound unterstützt wird, dann wird sich für
	// DirectSound entschieden. Dies wird anhand der sound_api_flags geprüft,
	// wobei hier nur die Bits 1 und 2 eine Rolle spielen.

	if((sound_api_flags & (DX9SND_OUTPUT | OPENAL_OUTPUT)) == 0x003)
		sound_api_flags &= DX9SND_OUTPUT;

	// API Flags zurückgeben
	return sound_api_flags;

}

void sndSetSoundApiFlags(int flags)
{
	sound_api_flags = flags;
}

int sndGetSoundApiFlags()
{
	return sound_api_flags;
}