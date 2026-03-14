//////////////////////////////////////////////////////////////////////////
// Billboarding (siehe Lighthouse3D Tutorial

#ifndef _BILLBOARDING_H_
#define _BILLBOARDING_H_

#include "math/vectormath.h"

GLvoid BillboardCylindricalBegin();
GLvoid BillboardSphericalBegin();
void BillboardGetUpRightVector(vector3f_t *up,vector3f_t *right);
void BillboardCylindricalExactBegin(float objPosX, float objPosY, float objPosZ);
void SetBillboardCamPos(vector3f_t *cp);
GLvoid BillboardEnd();

#endif _BILLBOARDING_H_