//////////////////////////////////////////////////////////////////////////
// Vektormathematik
//
//	Vergleiche C++ Implementierung "Physics for Game Programmers", O'Reilly


#include "../stdafx.h"
#include "quaternion.h"

// Neues Quaternion erzeugen
quaternion_t NewQuaternion(float w, float x, float y, float z)
{
	quaternion_t q = NewEmptyQuaternion();

	q.w  = w;
	q.v.x = x;
	q.v.y = y;
	q.v.z = z;

	return q;
}

quaternion_t NewQuatFromVect(float w, vector3f_t v)
{
	quaternion_t q = NewEmptyQuaternion();

	q.w = w;
	q.v = v;

	return q;
}

quaternion_t NewEmptyQuaternion()
{
	quaternion_t q;

	q.w = 0;
	q.v.x = 0;
	q.v.y = 0;
	q.v.z = 0;

	return q;
}

// Länge des Quaternions berechnen
float	QuatMagnitude(quaternion_t q)
{
	return (float) sqrtf(q.w * q.w + q.v.x * q.v.x + q.v.y * q.v.y + q.v.z * q.v.z);
}

// Quaternion Normalisieren
void	QuatNormalize(quaternion_t *q)
{
	float mag = (float) sqrtf(q->w * q->w + q->v.x * q->v.x + q->v.y * q->v.y + q->v.z * q->v.z);

	// Wenn die Länge des Vektors kleiner als eine Toleranzgrenze ist: 
	if(mag <= FLOAT_TYPE_TOLERANCE)
		mag = 1;
	
	q->w /= mag;
	q->v.x /= mag;
	q->v.y /= mag;
	q->v.z /= mag;

	// Genauigkeit der Werte testen.
	if(fabs(q->w) < FLOAT_TYPE_TOLERANCE)
		q->w = 0.0f;
	if(fabs(q->v.x) < FLOAT_TYPE_TOLERANCE)
		q->v.x = 0.0f;
	if(fabs(q->v.y) < FLOAT_TYPE_TOLERANCE)
		q->v.y = 0.0f;
	if(fabs(q->v.z) < FLOAT_TYPE_TOLERANCE)
		q->v.z = 0.0f;

}

// Konjugieren
quaternion_t  ConjugateQuat(quaternion_t q)
{
	return NewQuaternion(q.w, -q.v.x, -q.v.y, -q.v.z);
}

// Multiplizieren von Quaternionen
quaternion_t MultiplyQuat(quaternion_t q1, quaternion_t q2)
{
	quaternion_t q3  = NewEmptyQuaternion();

	q3.w   = q1.w*q2.w	 - q1.v.x*q2.v.x - q1.v.y*q2.v.y - q1.v.z*q2.v.z;

	q3.v.x = q1.w*q2.v.x + q1.v.x*q2.w	 + q1.v.y*q2.v.z - q1.v.z*q2.v.y;
	q3.v.y = q1.w*q2.v.y + q1.v.y*q2.w   + q1.v.z*q2.v.x - q1.v.x*q2.v.z;
	q3.v.z = q1.w*q2.v.z + q1.v.z*q2.w   + q1.v.x*q2.v.y - q1.v.y*q2.v.x;

	return q3;
}

//
// Quaternion q mit Vektor v multiplizieren
// 
// Normale Quaternionenmultiplikation indem v in ein Quaternion mit Skalarteil w = 0 konvertiert
// wird
//
quaternion_t MultiplyQuatVector(quaternion_t q, vector3f_t v)
{
	return NewQuaternion(-(q.v.x * v.x + q.v.y * v.y + q.v.z * v.z),
						  q.w * v.x    + q.v.y * v.z - q.v.z * v.y,
						  q.w * v.y    + q.v.z * v.x - q.v.x * v.z,
						  q.w * v.z    + q.v.x * v.y - q.v.y * v.x);
}

//
// Skalarmultiplikation. Jede Komponente des Quaternions wird mit s multipliziert
//
quaternion_t MultiplyQuatScalar(quaternion_t q, float s)
{
	return NewQuaternion(q.w * s, q.v.x * s, q.v.y * s, q.v.z * s);
}

// Quaternionen addieren
quaternion_t AddQuat(quaternion_t q1, quaternion_t q2)
{
	return NewQuaternion(q1.w + q2.w, q1.v.x + q2.v.x, q1.v.y + q2.v.y, q1.v.z + q1.v.z);
}
// Quaternionen subtrahieren
quaternion_t SubtractQuat(quaternion_t q1, quaternion_t q2)
{
	return NewQuaternion(q1.w - q2.w, q1.v.x - q2.v.x, q1.v.y - q2.v.y, q1.v.z - q1.v.z);
}

// Skalarkomponente ermitteln
float QuatGetScalar(quaternion_t q)
{
	return q.w;
}
// Vektorkomponente ermitteln
vector3f_t QuatGetVector(quaternion_t q)
{
	return q.v;
}

// Rotationswinkel um die Vektorkomponente des Quaternions ermitteln ist im
// Skalarteil enthalten (w = cos(angle/2))
float QuatGetAngle(quaternion_t q)
{
	return (float) (2*acos(q.w));
}

// Einheitsvektor der Rotationsachse ermitteln, die durch die Vektorkomponente 
// repräsentiert wird
vector3f_t QuatGetAxis(quaternion_t q)
{
	float m = VectMagnitude(q.v);

	if(m < FLOAT_TYPE_TOLERANCE)
		return q.v;
	else
		return DivideVector(q.v, m); // effizienter als NormalizeVector zu rufen
}

// Quaternion q1 mittels q2 rotieren
// Formel : q' = q1.q2.(q1*)
quaternion_t QuatRotate(quaternion_t q1, quaternion_t q2)
{
	return MultiplyQuat(MultiplyQuat(q1, q2), ConjugateQuat(q1));
}

// Vektor v um das Einheitsquaternion q rotieren
// Vektor to Quaternion: Skalarteil w = 0.
vector3f_t QuatVectRotate(quaternion_t q, vector3f_t v)
{
	quaternion_t w;
	
	w =  MultiplyQuat(MultiplyQuat(q, NewQuaternion(0, v.x, v.y, v.z)),
						ConjugateQuat(q));

	return QuatGetVector(w);
}

// Ermittelt das der Rotation um den Vektor v entsprechende Rotationsquaternion
// Siehe Mathematische Grundlagen Skript - C.Nolte
quaternion_t GetRotationQuat(float angle, vector3f_t v)
{
	// Sicherstellen das v normalisiert ist
	VectNormalize(&v);

	// Berechnen des Sinus der Hälfte des Rotationswinkels (entsprechend der
	// Rotationsformel des Quaternions
	angle = DEGTORAD(angle);
	float sine = sinf( angle / 2.0f);

	// Quaternion erzeugen und normalisieren
	quaternion_t q = NewQuaternion(cosf( angle / 2.0f), v.x * sine, v.y * sine, v.z * sine);
	QuatNormalize(&q);
	return q;
}

// Rotations-Quaternion aus Euler Winkeln erzeugen
// x, y, z repräsentieren hier die Rotationswinkel in einem Euler'schen Koordinaten-
// system. Um aus diesen 3 kombinierten Rotationen ein Rotationsquaternion zu
// erzeugen, werden 3 Quaternionen erstellt und diese dann miteinander multipliziert
// Um Overhead zu vermeiden ist diese Funktion bereits optimiert und verwendet
// lediglich ein Quaternion und führt die Berechnung mit Hilfsvariablen durch
quaternion_t MakeQuatFromEulerAngles(float x, float y, float z)
{
	quaternion_t q = NewEmptyQuaternion();

	double roll		= DEGTORAD(x);
	double pitch	= DEGTORAD(y);
	double yaw		= DEGTORAD(z);

	double cyaw, cpitch, croll, syaw, spitch, sroll;
	double cyawcpitch, syawspitch, cyawspitch, syawcpitch;

	cyaw	= cos(0.5f * yaw);
	cpitch	= cos(0.5f * pitch);
	croll	= cos(0.5f * roll);
	syaw	= sin(0.5f * yaw);
	spitch  = sin(0.5f * pitch);
	sroll	= sin(0.5f * roll);

	cyawcpitch = cyaw * cpitch;
	syawspitch = syaw * spitch;
	cyawspitch = cyaw * spitch;
	syawcpitch = syaw * cpitch;

	q.w   = (float) (cyawcpitch * croll + syawspitch * sroll);
	q.v.x = (float) (cyawcpitch * sroll - syawspitch * croll);
	q.v.y = (float) (cyawspitch * croll + syawcpitch * sroll);
	q.v.z = (float) (syawcpitch * croll - cyawspitch * sroll);

	return q;


}

// Euler Winkel aus der Rotationsmatrix (die aus dem Quaternion gewonnen wird
// extrahieren
vector3f_t   MakeEulerAnglesFromQuat(quaternion_t q)
{
	double r11, r21, r31, r32, r33, r12, r13;
	double tmp;

	vector3f_t v;
	
	// Wegen besserer Lesbarkeit geklammert
	r11 = (q.w * q.w) + (q.v.x * q.v.x) - (q.v.y * q.v.y) - (q.v.z * q.v.z);
	r21 = 2 * (q.v.x * q.v.y + q.w * q.v.z);
	r31 = 2 * (q.v.x * q.v.z - q.w * q.v.y);
	r32 = 2 * (q.v.y * q.v.z + q.w * q.v.x);
	r33 = (q.w * q.w) - (q.v.x * q.v.x) - (q.v.y * q.v.y) + (q.v.z * q.v.z);

	tmp = fabs(r31);  // fabs kalkuliert den Betrag von floating-point Werten

	if(tmp > 0.999999) //  TODO: Klären warum ?
	{
		r12 = 2 * (q.v.x * q.v.y - q.w * q.v.z);
		r13 = 2 * (q.v.x * q.v.z + q.w * q.v.y);

		v.x = RADTODEG(0.0f);							//roll
		v.y = RADTODEG((float) (-(PI/2) * r31 / tmp) ); // pitch
		v.z = RADTODEG((float) atan2(-r12, -r31*r13) ); // yaw

		return v;
	}

	v.x = RADTODEG( (float) atan2(r32, r33) );			// roll, atan2: arctangens von y/x
	v.y = RADTODEG( (float) asin(-r31) );				// pitch
	v.z = RADTODEG( (float) atan2(r21, r11) );			// yaw

	return v;
}