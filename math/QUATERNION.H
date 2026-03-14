// Vergleiche "Physics For Game Programmers", O'Reilly (C++ Implementation)
//

#ifndef _QUATERNION_H
#define _QUATERNION_H

#include "mathtoolkit.h"
#include "vectormath.h"

// Ein Quaternion
typedef struct {
	float w;		// Skalar
	vector3f_t v;	// Vektorteil
} quaternion_t;

// Neues Quaternion erzeugen
quaternion_t NewQuaternion(float w, float x, float y, float z);
quaternion_t NewQuatFromVect(float w, vector3f_t v);
quaternion_t NewEmptyQuaternion();

// Länge des Quaternions berechnen
float	QuatMagnitude(quaternion_t q);
// Quaternion Normalisieren
void	QuatNormalize(quaternion_t *q);
// Ermittelt das der Rotation um den Vektor v entsprechende Rotationsquaternion
// Siehe Mathematische Grundlagen Skript - C.Nolte
quaternion_t GetRotationQuat(float angle, vector3f_t v);
// Quaternion konjugieren
quaternion_t  ConjugateQuat(quaternion_t q);
// Multiplizieren von Quaternionen
quaternion_t MultiplyQuat(quaternion_t q1, quaternion_t q2);
quaternion_t MultiplyQuatVector(quaternion_t q, vector3f_t v);
quaternion_t MultiplyQuatScalar(quaternion_t q, float s);

// Quaternionen addieren
quaternion_t AddQuat(quaternion_t q1, quaternion_t q2);
// Quaternionen subtrahieren
quaternion_t SubtractQuat(quaternion_t q1, quaternion_t q2);


// Skalarkomponente ermitteln
float QuatGetScalar(quaternion_t q);
// Vektorkomponente ermitteln
vector3f_t QuatGetVector(quaternion_t q);

// Rotationsfunktionen

// Rotationswinkel um die Vektorkomponente des Quaternions ermitteln
float QuatGetAngle(quaternion_t q);
// Einheitsvektor der Rotationsachse ermitteln, die durch die Vektorkomponente repräsentiert wird
vector3f_t QuatGetAxis(quaternion_t q);
// Quaternion q1 um q2 rotieren
quaternion_t QuatRotate(quaternion_t q1, quaternion_t q2);
// Vektor v um das Einheitsquaternion q rotieren
vector3f_t QuatVectRotate(quaternion_t q, vector3f_t v);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
// Quaternnion aus Euler Winkeln erzeugen
quaternion_t MakeQuatFromEulerAngles(float x, float y, float z);
// Euler Winkel aus der Rotationsmatrix (die aus dem Quaternion gewonnen wird
// extrahieren
vector3f_t   MakeEulerAnglesFromQuat(quaternion_t q);

// Algorithmen
void Slerp(float t, const quaternion_t from, const quaternion_t to);

#endif /*_QUATERNION_H */


