// Vergleiche "Physics For Game Programmers", O'Reilly (C++ Implementation)
//

#ifndef _MATRIX3X3_H
#define _MATRIX3X3_H

#include "vectormath.h"

// Matrix
typedef struct 
{
	float a11, a12, a13;
	float a21, a22, a23;
	float a31, a32, a33;
} matrix3x3_t;

matrix3x3_t NewMatrix3x3();
matrix3x3_t InitMatrix3x3(float a11, float a12, float a13,
						   float a21, float a22, float a23,
						   float a31, float a32, float a33);
float det3x3(matrix3x3_t m);
matrix3x3_t InverseMatrix(matrix3x3_t m);
matrix3x3_t Transpose3x3(matrix3x3_t m);
matrix3x3_t AddMatrix(matrix3x3_t m, matrix3x3_t n);
vector3f_t MatrixMultVector(matrix3x3_t m, vector3f_t v);

#endif  _MATRIX3X3_H
