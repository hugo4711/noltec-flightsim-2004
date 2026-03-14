//////////////////////////////////////////////////////////////////////////
// Header-File frustum.h

#ifndef _FRUSTUM_H
#define _FRUSTUM_H

#include "math/vectormath.h"

typedef struct 
{
	// The elements of the 4x4 matrix are stored in
	// column-major order (see "OpenGL Programming Guide",
	// 3rd edition, pp 106, glLoadMatrix).
	float _11, _21, _31, _41;
	float _12, _22, _32, _42;
	float _13, _23, _33, _43;
	float _14, _24, _34, _44;
} matrix4x4_t;

// Definiert den Halbraum um zu testen, ob ein Punkt auf
// einem Plane oder davor oder dahinter liegt.
typedef enum
{
	NEGATIVE = -1,
	ON_PLANE = 0,
	POSITIVE = 1,
} halfspace_t;

typedef struct  
{
	float a, b, c, d;
} clipping_plane_t;

matrix4x4_t ClipMatrixModelSpace(float *m, float *p);

void ExtractPlanesGL(clipping_plane_t * p_planes,
					 const matrix4x4_t & comboMatrix,
					 bool normalize);

halfspace_t ClassifyPoint(const clipping_plane_t & plane, const vector3f_t & pt);
void GetFrustumPanesMS(clipping_plane_t *planes);
bool isPointVisibleOnPlane(const clipping_plane_t *planes, const vector3f_t & pt);

//////////////////////////////////////////////////////////////////////////
// TEST
//////////////////////////////////////////////////////////////////////////
/*
bool isPointVisible(float x, float y, float z);
void CalculateFrustum(float *md, float *proj);
void NormalizeFrustum();
void setFrustum(clipping_plane_t *p_planes);
*/

#endif /* _FRUSTUM_H */