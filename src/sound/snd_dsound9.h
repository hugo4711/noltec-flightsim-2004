//////////////////////////////////////////////////////////////////////////
// Soundunterst�tzung Direct Sound 9

#ifndef _SND_DSOUND9_H
#define _SND_DSOUND9_H

// Direct Sound 9 Support (Bentigt DirectX 9 SDK)
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dsound.lib")

#include <dsound.h>
#include "../math/vectormath.h"

// Dieses Struct definiert ein Soundobjekt fr eine Sounddatei
typedef struct {
	void* pSegment; // Stubbed
	void* p3DAudioPath; // Stubbed
	IDirectSound3DBuffer8* pDSB;
} sndDX9_soundobject_t;

sndDX9_soundobject_t **sndDX9InitSoundGeneral();

BOOL sndDX9InitSound2D();
BOOL sndDX9InitSound3D();
BOOL sndDX9CleanUp();
BOOL sndDX9LoadSoundfile(WCHAR *pwsFileName, sndDX9_soundobject_t *sndobject);
BOOL sndDX9CleanUpSndObject(sndDX9_soundobject_t *sndobject);
BOOL sndDX9PlaySoundAt(vector3f_t listenerpos,			// Position des Zuh�hrers
					   vector3f_t soundpos,			// Position des Ger�uschs
					   sndDX9_soundobject_t *sndobject,
					   BOOL loop);
BOOL sndDX9SetListenerPos(float panLR,	//pan left/right
						  float panNF,  //pan near/far
						  float panHL);

BOOL sndDX9SetSoundSourcePos(sndDX9_soundobject_t *sndobject,
							 float x,	// x-Achse
							 float y,  //y-Achse
							 float z);

#endif _SND_DSOUND9_H


