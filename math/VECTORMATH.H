//////////////////////////////////////////////////////////////////////////
// Vektor Mathematik
//
// Vergleiche "Physics For Game Programmers", O'Reilly (C++ Implementation)
//
// Informationen über Vektorrechnung bietet das Buch 
// "Mathematik für Ingenieure und Naturwissenschaftler Band 1", Lothar Papula, Vieweg

#ifndef _VECTORMATH_H
#define _VECTORMATH_H

#include "mathtoolkit.h"

// Ein Vektor
typedef struct {
	float x;
	float y;
	float z;
} vector3f_t;

// Neuen Vektor erzeugen
vector3f_t NewVector(float x, float y, float z);
// Neuer "leerer" Vektor
vector3f_t NewEmptyVector();

// Länge des Vektors berechnen
float	VectMagnitude(vector3f_t v);
// Vektor Normalisieren
void	VectNormalize(vector3f_t *v);
// Vektor umdrehen
vector3f_t	VectInvert(vector3f_t v);
// Vektoren addieren
vector3f_t AddVector(vector3f_t u, vector3f_t v);
// Vektoren subtrahieren
vector3f_t SubtractVector(vector3f_t u, vector3f_t v);
// Skalarprodukt bilden
float VectDotProduct(vector3f_t u, vector3f_t v);
// Vektor durch Skalar dividieren
vector3f_t DivideVector(vector3f_t u, float s);
// Kreuzprodukt bilden
vector3f_t VectCrossProduct(vector3f_t u, vector3f_t v);
// Winkel zwischen den beiden Vektoren bestimmen
float GetVectorAngle(vector3f_t u, vector3f_t v);
// Skalarmultiplikation
vector3f_t VectScalarMult(vector3f_t v, float s);

#endif /*_VECTORMATH_H */


