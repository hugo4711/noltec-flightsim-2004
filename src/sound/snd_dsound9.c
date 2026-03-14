#include "../stdafx.h"
#include "snd_dsound9.h"
#include "snd_general.h"

BOOL sndDX9InitSound2D() { return FALSE; }
BOOL sndDX9InitSound3D() { return FALSE; }
BOOL sndDX9CleanUp() { return TRUE; }
BOOL sndDX9LoadSoundfile(WCHAR *pwsFileName, sndDX9_soundobject_t *sndobject) { return FALSE; }
BOOL sndDX9CleanUpSndObject(sndDX9_soundobject_t *sndobject) { if(sndobject) free(sndobject); return TRUE; }
BOOL sndDX9PlaySoundAt(vector3f_t listenerpos, vector3f_t soundpos, sndDX9_soundobject_t *sndobject, BOOL loop) { return FALSE; }
BOOL sndDX9SetListenerPos(float panLR, float panNF, float panHL) { return FALSE; }
BOOL sndDX9SetSoundSourcePos(sndDX9_soundobject_t *sndobject, float x, float y, float z) { return FALSE; }
sndDX9_soundobject_t **sndDX9InitSoundGeneral() { return NULL; }
