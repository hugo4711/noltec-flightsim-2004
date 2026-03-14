//////////////////////////////////////////////////////////////////////////
// Kapseln aller Kameraoperationen

#ifndef _CAMERA_H
#define _CAMERA_H

#define MOVE_SPEED				10.0f			// Größerer Wert = höhere Bewegungsgeschwindigkeit
#define MOUSE_SPEED_DIVISOR		10.0f			// Kleinerer Wert = höhere Mausspeed

#include "math/vectormath.h"	// Vektormathematik
#include "math/quaternion.h"	// Quaternionenmathematik
#include "sound/snd_general.h"	// Für die 3D-Positionierung des Sounds

// Kamera mit der Maus bewegen
GLvoid SetCameraViewByMouse();
// View rotieren
GLvoid RotateCamera(float angle, float x, float y, float z);
// Quaternionenkamera
GLvoid RotateCameraQ(float angle,float x, float y, float z);

GLvoid InitCamera(vector3f_t pos, vector3f_t lookat, vector3f_t up);
GLvoid SetCameraVectors(vector3f_t pos, vector3f_t lookat, vector3f_t up);
//GLvoid GetCameraVectors(vector3f_t *pos, vector3f_t *lookat, vector3f_t *up);
vector3f_t GetCameraPosition();
vector3f_t GetCameraView();
vector3f_t GetCameraUp();

GLvoid SetScreenDimensions(int height, int width);

GLvoid FreeCamera();

#endif /*_CAMERA_H_ */


