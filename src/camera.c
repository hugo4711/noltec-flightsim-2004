//////////////////////////////////////////////////////////////////////////
// Implementation der Kamerasteuerung

#include "stdafx.h"
#include "camera.h"
#include "glscene.h"

// Die drei Kameravektoren
static vector3f_t *vCamView = NULL;
static vector3f_t *vCamPosition = NULL;
static vector3f_t *vCamUp = NULL;
// Bildschirmausdehnung in Pixeln
static int screen_width, screen_height;


//
//	NAME: RotateCamera
//
// ZWECK: This rotates the view around the position using an axis-angle rotation
//
GLvoid RotateCamera(float angle,	// Grad der Rotation um den Vektor
					float x,		// um diesen Punkt wird rotiert
					float y,
					float z)
{
	vector3f_t vNewView = NewEmptyVector();

	// Get the view vector (The direction we are facing)
	vector3f_t vView = SubtractVector(*vCamView, *vCamPosition);		

	// Calculate the sine and cosine of the angle once
	float cosTheta = (float) cos(DEGTORAD(angle));
	float sinTheta = (float) sin(DEGTORAD(angle));

	// Find the new x position for the new rotated point
	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

	// Find the new y position for the new rotated point
	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

	// Find the new z position for the new rotated point
	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;

	// Now we just add the newly rotated vector to our position to set
	// our new rotated view of our camera.
	*vCamView = AddVector(*vCamPosition, vNewView);
}

//
//	NAME: RotateCameraQ
//
// ZWECK: Kamera mittels Quaternionen rotieren
//
GLvoid RotateCameraQ(float angle,	// Grad der Rotation um den Vektor
					 float x,		// um diesen Punkt wird rotiert
					 float y,
					 float z)
{
	quaternion_t qRotation, qView, qNewView;

	// Rotationsquaternion aus den Rotationsdaten erzeugen
	qRotation = GetRotationQuat(angle, NewVector(x, y, z));

	// Aus dem View-Vektor unde dem Positionsvektor der Kamera wird 
	// das entsprechende Distanz-Quaternion erzeugt
	qView = NewQuaternion(0, vCamView->x - vCamPosition->x,
		vCamView->y - vCamPosition->y,
		vCamView->z - vCamPosition->z);

	// Rotation entsprechend der Formel
	//		q' = q1*q2*(-q1)
	// berechnen
	qNewView = MultiplyQuat(MultiplyQuat(qRotation, qView), ConjugateQuat(qRotation));

	//View-Vektor der Kamera aktualisieren
	vCamView->x = vCamPosition->x + qNewView.v.x;
	vCamView->y = vCamPosition->y + qNewView.v.y;
	vCamView->z = vCamPosition->z + qNewView.v.z;


}

//
// NAME :  MoveCamera
//
// ZWECK:  Kamera in Blickrichtung vorw�rts und r�ckw�rts bewegen.
//
// PARAM: speed			  - Geschwindigkeit der Bewegung
//
GLvoid MoveCamera(float speed)
{
	// Get the current view vector (the direction we are looking)
	vector3f_t vVector = SubtractVector(*vCamView, *vCamPosition);
	VectNormalize(&vVector);

	vCamPosition->x += vVector.x * speed;		// Add our acceleration to our position's X
	vCamPosition->y += vVector.y * speed;
	vCamPosition->z += vVector.z * speed;		// Add our acceleration to our position's Z
	vCamView->x += vVector.x * speed;			// Add our acceleration to our view's X
	vCamView->y += vVector.y * speed;
	vCamView->z += vVector.z * speed;			// Add our acceleration to our view's Z

	// Listenerposition entsprechend anpassen da sich der Zuh�hrer 
	// an der Kameraposition befindet
	// Pr�fen welche Soundengine genutzt wird
	if((sndGetSoundApiFlags() & DX9SND_OUTPUT) == DX9SND_OUTPUT )
	{
		// Achtung Koordinatensystem beim Panning "vertauscht" (siehe Header-File)
		// l/r (x), n/f (y), h/l (z)
		sndDX9SetListenerPos(vCamPosition->x,vCamPosition->z, vCamPosition->y);
	}

}

//
//	NAME : SetCameraViewByMouse
//
// ZWECK : Camera mit der Maus bewegen
//
//
GLvoid SetCameraViewByMouse()
{
	POINT mousePos;									// This is a window structure that holds an X and Y
	int middleX = screen_width  >> 1;				// This is a binary shift to get half the width
	int middleY = screen_height >> 1;				// This is a binary shift to get half the height
	float deltaX = 0.0f;
	float deltaY = 0.0f;

	// Get the mouse's current X,Y position (WIN32) 
	GetCursorPos(&mousePos);						

	// If our cursor is still in the middle, we never moved... so don't update the screen
	if( (mousePos.x == middleX) && (mousePos.y == middleY) ) return;

	// Set the mouse position to the middle of our window (WIN32)
	SetCursorPos(middleX, middleY);							

	// Get the direction the mouse moved in
	deltaX = (float) (middleX - mousePos.x);		
	deltaY = (float) (middleY - mousePos.y);		

	// Steuerung des Flugzeugs basierend auf Mausbewegung
	if (deltaY > 10.0f) PitchDown();
	else if (deltaY < -10.0f) PitchUp();

	if (deltaX > 10.0f) RollLeft();
	else if (deltaX < -10.0f) RollRight();

}

// Kamera initialisieren
GLvoid InitCamera(vector3f_t pos, vector3f_t lookat, vector3f_t up)
{
	vCamPosition = (vector3f_t*)malloc(sizeof(vector3f_t));
	vCamView = (vector3f_t*)malloc(sizeof(vector3f_t));
	vCamUp = (vector3f_t*)malloc(sizeof(vector3f_t));
	
	assert( (vCamPosition || vCamView || vCamUp) && "InitCamera: Memory allocation failed.");

	SetCameraVectors(pos, lookat, up);
	
}

GLvoid SetCameraVectors(vector3f_t pos, vector3f_t lookat, vector3f_t up)
{
	if(!vCamPosition || !vCamView || !vCamUp)
		return;

	*vCamPosition = pos;
	*vCamView = lookat;
	*vCamUp = up;

}
/*
// TODO: Das zur�ckgeben der Referenzen funtkioniert nicht
GLvoid GetCameraVectors(vector3f_t *pos, vector3f_t *lookat, vector3f_t *up)
{
	pos = vCamPosition;
	lookat = vCamView;
	up = vCamUp;
}
*/
vector3f_t GetCameraPosition()
{
	return *vCamPosition;
}

vector3f_t GetCameraView()
{
	return *vCamView;
}

vector3f_t GetCameraUp()
{
	return *vCamUp;
}

GLvoid SetScreenDimensions(int height, int width)
{
	screen_width = width;
	screen_height = height;
}

//
// Speicher der f�r die Kameravektoren angelegt wurde, wieder freigeben
//
GLvoid FreeCamera()
{
	free(vCamView);
	free(vCamPosition);
	free(vCamUp);
}