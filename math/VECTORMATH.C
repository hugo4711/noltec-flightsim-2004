//////////////////////////////////////////////////////////////////////////
// Vektormathematik
//
//	Vergleiche C++ Implementierung "Physics for Game Programmers", O'Reilly
// Informationen �ber Vektorrechnung bietet das Buch 
// "Mathematik f�r Ingenieure und Naturwissenschaftler Band 1", Lothar Papula, Vieweg

#include "../stdafx.h"
#include "vectormath.h"

//
// NAME: NewVector
//
// ZWECK: Neuen Vektor erzeugen
//
vector3f_t NewVector(float x, float y, float z)
{
	vector3f_t v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}
//
// ZWECK: Neuer "leerer" Vektor
//
vector3f_t NewEmptyVector()
{
	vector3f_t v;
	v.x = 0;
	v.y = 0;
	v.z = 0;
	return v;
}

//
// ZWECK: L�nge des Vektors berechnen
//
float	VectMagnitude(vector3f_t v)
{
	// Satz des Pythagoras
	return (float) sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

//
// ZWECK: Vektor Normalisieren
//
void	VectNormalize(vector3f_t *v)
{
	// L�nge des Vektors
	float mag = (float) sqrtf(v->x*v->x + v->y*v->y + v->z*v->z) ;
	
	// Wenn die L�nge des Vektors kleiner als eine Toleranzgrenze ist: 
	if(mag <= FLOAT_TYPE_TOLERANCE)
		mag = 1;

	v->x /= mag;
	v->y /= mag;
	v->z /= mag;

	// Genauigkeit der Werte testen.
	if(fabs(v->x) < FLOAT_TYPE_TOLERANCE)
		v->x = 0.0f;
	if(fabs(v->y) < FLOAT_TYPE_TOLERANCE)
		v->y = 0.0f;
	if(fabs(v->z) < FLOAT_TYPE_TOLERANCE)
		v->z = 0.0f;

}

//
// ZWECK: Vektor invertieren
//
vector3f_t	VectInvert(vector3f_t v)
{
	vector3f_t u;
	
	u.x = -v.x;
	u.y = -v.y;
	u.z = -v.z;

	return u;
}

//
// ZWECK: Vektoren v+u addieren
//
vector3f_t AddVector(vector3f_t u, vector3f_t v)
{
	vector3f_t w;

	w.x = u.x + v.x;
	w.y = u.y + v.y;
	w.z = u.z + v.z;

	return w;
}

//
//	ZWECK: Vektoren v+(-u) addieren = subtrahieren
//
vector3f_t SubtractVector(vector3f_t u, vector3f_t v)
{
	vector3f_t w;
	w.x = u.x - v.x;
	w.y = u.y - v.y;
	w.z = u.z - v.z;

	return w;
}

// 
// Name : VectDotProduct
//
// Zweck: Berechnet das Punkt Produkt. 
//
float VectDotProduct(vector3f_t u, vector3f_t v)
{
	return  (u.x * v.x + u.y * v.y + u.z * v.z);
}

// Vektor durch Skalar dividieren
vector3f_t DivideVector(vector3f_t u, float s)
{
	return NewVector(u.x / s, u.y /s , u.z / s);
}

//
//	NAME : VectCrossProduct
//
// ZWECK : Kreuzprodukt zweier Vektoren bilden
//		   
//		   Der Ergebnisvektor entspricht dem Normalvektor der berechneten Fl�che, da er
//		   orhogonal (90� Winkel) zu u UND GLEICHZEIGTIG v ist.
//		   Der Betrag (die L�nge des Vektors) des Kreuzprodukts (auch Vektorprodukt) entspricht
//		   dem Fl�cheninhalt des von den Vektoren u und v aufgespannten Parallelogramms.
//
vector3f_t VectCrossProduct(vector3f_t u, vector3f_t v)
{
	vector3f_t w;

	w.x =  u.y*v.z - u.z*v.y; 
	w.y =  u.z*v.x - u.x*v.z;
	w.z =  u.x*v.y - u.y*v.x;

	return w;
}


//
//	ZWECK: Winkel zwischen Vektor u und v bestimmen
//
float GetVectorAngle(vector3f_t u, vector3f_t v)
{
	// Skalarprodukt geteilt durch das Produkt der Vektorl�ngen gibt den Winkel zw. u und v
	return (u.x * v.x + u.y * v.y + u.z * v.z) / 
		   (float) sqrtf(v.x*v.x + v.y*v.y + v.z*v.z) *
		   (float) sqrtf(u.x*u.x + u.y*u.y + u.z*u.z);
}

//
//	Multipliziert den Vektor v mit dem Skalar s
//
vector3f_t VectScalarMult(vector3f_t v, float s)
{
	vector3f_t u;

	u.x = v.x * s;
	u.y = v.y * s;
	u.z = v.z * s;

	return u;
}