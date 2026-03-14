//////////////////////////////////////////////////////////////////////////
// Matriximplementation
//
//	Vergleiche C++ Implementierung "Physics for Game Programmers", O'Reilly

#include "../stdafx.h"
#include "matrix3x3.h"

// Neue Matrix initialisieren
matrix3x3_t NewMatrix3x3()
{
	matrix3x3_t m;

	m.a11 = 0.0f;
	m.a12 = 0.0f;
	m.a13 = 0.0f;
	m.a21 = 0.0f;
	m.a22 = 0.0f;
	m.a23 = 0.0f;
	m.a31= 0.0f;
	m.a32 = 0.0f;
	m.a33 = 0.0f;

	return m;
}

// Initialisieren mit eigenen Werten
matrix3x3_t InitMatrix3x3(float a11, float a12, float a13,
						   float a21, float a22, float a23,
						   float a31, float a32, float a33)
{
	matrix3x3_t m;

	m.a11 = a11;
	m.a12 = a12;
	m.a13 = a13;
	m.a21 = a21;
	m.a22 = a22;
	m.a23 = a23;
	m.a31 = a31;
	m.a32 = a32;
	m.a33 = a33;

	return m;
}

// Determinante berechnen
float det3x3(matrix3x3_t m)
{
	return m.a11 * m.a22 * m.a33 -
			m.a11 * m.a32 * m.a23 +
			m.a21 * m.a32 * m.a13 -
			m.a21 * m.a12 * m.a33 +
			m.a31 * m.a12 * m.a23 -
			m.a31 * m.a22 * m.a13;
}

// Inverse der Matrix m bestimmen
matrix3x3_t InverseMatrix(matrix3x3_t m)
{

	float d = m.a11 * m.a22 * m.a33 -
			m.a11 * m.a32 * m.a23 +
			m.a21 * m.a32 * m.a13 -
			m.a21 * m.a12 * m.a33 +
			m.a31 * m.a12 * m.a23 -
			m.a31 * m.a22 * m.a13;

	if(d==0) d = 1;

	return InitMatrix3x3( (m.a22*m.a33-m.a23*m.a32) / d,
		-(m.a12*m.a33-m.a13*m.a32) / d,
		(m.a12*m.a23-m.a13*m.a22) / d,
		-(m.a21*m.a33-m.a23*m.a31) / d,
		(m.a11*m.a33-m.a13*m.a31) / d,
		-(m.a11*m.a23-m.a13*m.a21) / d,
		(m.a21*m.a32-m.a22*m.a31) / d,
		-(m.a11*m.a32-m.a12*m.a31) / d,
		(m.a11*m.a22-m.a12*m.a21) /d );
}

// Matrix transponieren
matrix3x3_t Transpose3x3(matrix3x3_t m)
{
	return InitMatrix3x3(m.a11,m.a21,m.a31,
						 m.a12,m.a22,m.a32,
						 m.a13,m.a23,m.a33);
}

// Matrizen addieren
matrix3x3_t AddMatrix(matrix3x3_t m, matrix3x3_t n)
{
	return InitMatrix3x3(m.a11+n.a11, m.a12+n.a12, m.a13+n.a13,
					     m.a21+n.a21, m.a22+n.a22, m.a23+n.a23,
					     m.a31+n.a31, m.a32+n.a32, m.a33+n.a33);
}

//
// Matrix m mit vector v multiplizieren
//
vector3f_t MatrixMultVector(matrix3x3_t m, vector3f_t v)
{
	return NewVector( m.a11*v.x + m.a12*v.y + m.a13*v.z,
					  m.a21*v.x + m.a22*v.y + m.a23*v.z,
					  m.a31*v.x + m.a32*v.y + m.a33*v.z);
}