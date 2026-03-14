//////////////////////////////////////////////////////////////////////////
// Soundunterstützung (Generelle Definitionen die für das Laden der Sounds
// benötigt werden)

#ifndef _SND_GENERAL_H
#define _SND_GENERAL_H

#include "snd_dsound9.h"
#include "snd_openal.h"

// Anhand dieser Konstanten wird entschieden, welche Sound-Api verwendet werden soll
// Dies dient zur Definition von Flags die per Bitweisen UND/ODER entsprechend manipuliert werden
// Bit 1 = OpenAL wird von der Soundkarte unterstüzt
// Bit 2 = DirectSound wird unterstützt
#define OPENAL_OUTPUT		0x001
#define DX9SND_OUTPUT		0x002

// Anzahl der Sounddateien
// TODO: In eine Konfigurationsdatei auslagern
#define SOUND_CNT		4

#define SOUND_ENGINE1_IDX			0						// Index
#define SOUND_ENGINE1_NAME			L"sound/jet_low.wav"	// wchar Dateiname
#define SOUND_MISSILELAUNCH_IDX		1
#define SOUND_MISSILELAUNCH_NAME	L"sound/missilelaunch.wav"
#define SOUND_BGMUSIC_IDX			2
#define SOUND_BGMUSIC_NAME			L"sound/music/background.wav"
#define SOUND_STALL_IDX				3
#define SOUND_STALL_NAME			L"sound/warning.wav"		

int sndGeneralSoundInit();
void sndSetSoundApiFlags(int flags);
int sndGetSoundApiFlags();

#endif _SND_GENERAL_H


