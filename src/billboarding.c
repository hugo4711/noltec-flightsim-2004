//////////////////////////////////////////////////////////////////////////
// Billboarding

#include "stdafx.h"
#include "billboarding.h"

vector3f_t *camera_position = NULL;


//
// Diese Funktion wird f�r die exakte Version des Billboardings ben�tigt,
// damit immer in Relation zur Kameraposition die Rotation des Objekts
// durchgef�hrt werden kann.
//
// Bekommt als Parameter die aktuelle Kameraposition �bergeben
//
void SetBillboardCamPos(vector3f_t *cp)
{
	camera_position = cp;
}

//
// Exaktes, cylindrisches Billboarding in relation zur Kameraposition
// siehe: Lighthouse3D Tutorial
//
// TODO: Funktion muss noch getestet werden. Funktionierte bisher noch nicht.
//		 Der Up-Vektor ist immer 0,0,0...
//
void BillboardCylindricalExactBegin(float objPosX, float objPosY, float objPosZ) 
{

	vector3f_t lookAt,objToCamProj,upAux;
	float angleCosine;

	// Pr�fen ob die Kameraposition zuvor initialisiert wurde
	assert(camera_position != NULL);
	if(!camera_position)
		return;

	glPushMatrix();

	// objToCamProj is the vector in world coordinates from the 
	// local origin to the camera projected in the XZ plane
	objToCamProj.x = camera_position->x - objPosX ;
	objToCamProj.y = 0;
	objToCamProj.z = camera_position->z - objPosZ ;

	// This is the original lookAt vector for the object 
	// in world coordinates
	lookAt.x = 0;
	lookAt.y = 0;
	lookAt.z = 1;


	// normalize both vectors to get the cosine directly afterwards
	VectNormalize(&objToCamProj);

	// easy fix to determine wether the angle is negative or positive
	// for positive angles upAux will be a vector pointing in the 
	// positive y direction, otherwise upAux will point downwards
	// effectively reversing the rotation.

	upAux = VectCrossProduct(lookAt, objToCamProj);
	//mathsCrossProduct(upAux,lookAt,objToCamProj);

	// compute the angle
	angleCosine = VectDotProduct(lookAt, objToCamProj);
	//angleCosine = mathsInnerProduct(lookAt,objToCamProj);

	// perform the rotation. The if statement is used for stability reasons
	// if the lookAt and objToCamProj vectors are too close together then 
	// |angleCosine| could be bigger than 1 due to lack of precision
	if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
		glRotatef(RADTODEG(acosf(angleCosine)),upAux.x, upAux.y, upAux.z);
}


//
//	Schnelle Methode des cylindrischen Billboardings. Die Objekte werden NICHT in relation
//  zum Kamerawinkel gekippt. Wenn man direkt �ber dem Objekt ist und
//  von oben herab darauf schaut, dann sieht man es nicht mehr.
//
GLvoid BillboardCylindricalBegin() {

	float modelview[16];
	int i,j;

	// save the current modelview matrix
	glPushMatrix();

	// get the current modelview matrix
	glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

	for( i=0; i<3; i+=2 ) 
		for( j=0; j<3; j++ ) {
			if ( i==j )
				modelview[i*4+j] = 1.0;
			else
				modelview[i*4+j] = 0.0;
		}

		// set the modelview matrix
		glLoadMatrixf(modelview);
}


//
// Schnelle Version des cylindrischen Billboardings.
// Vorteil ist, das pro Frame nur EINMAL die Werte aus der Modelview-Matrix geholt werden
// m�ssen. Das macht dieses Verfahren recht effizient.
//
//
// Nachteil: Manuelle Transformation
// der verticies mit folgenden Formeln:
// (siehe Lighthouse3d.com)
//First a quad with the center at the bottom origin is presented (left figure). 
//
//a = center - right * (size * 0.5);
//b = center + right * size * 0.5;
//c = center + right * size * 0.5 + up * size;
//d = center - right * size * 0.5 + up * size;
//
//The right figure represents a quad with a centre on the middle of the quad. 
// Its vertices are defined as: 
//a = center - (right + up) * size;
//b = center + (right - up) * size;
//c = center + (right + up) * size;
//d = center - (right - up) * size;
//
//
//
void BillboardGetUpRightVector(vector3f_t *up,vector3f_t *right) 
{

	float modelview[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

	assert(up && right);

	// For cylindrical billboarding (e.g. trees), we want the right vector 
	// to be horizontal in world space, and the up vector to be always (0,1,0).
	// The first row of the modelview matrix (transposed) is the camera's right vector.
	right->x = modelview[0];
	right->y = 0.0f;
	right->z = modelview[8];
	VectNormalize(right);
	
	up->x = 0.0f;
	up->y = 1.0f;
	up->z = 0.0f;
	
}

//
// Sph�rische BIllboards (siehe Lighthouse3D-Tutorial)
//
// Alle Objekte die zwischen BillboardSphericalBegin() und BillboardSphericalEnd stehen
// werden immer zur Kamera ausgerichtet und auch in relation zur Kamera gekippt.
//
// Achtung! Skalieren funktioniert mit dieser Variante NUR NACH BillboardSphericalBegin!
// 
GLvoid BillboardSphericalBegin() 
{

	float modelview[16];
	int i,j;

	// Aktuelle Modelviewmatrix sichern
	glPushMatrix();

	// Aktuelle Werte der Modelview Matrix holen
	glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

	// "R�ckg�ngig machen" aller Rotationen durch das 
	// �berschreiben des 3x3 "Teils" der Matrix mit der Einheitsmatrix
	// Dadurch werden Skalierungen ebenfalls r�ckg�ngig gemacht
	for( i=0; i<3; i++ ) 
		for( j=0; j<3; j++ ) {
			if ( i==j )
				modelview[i*4+j] = 1.0;
			else
				modelview[i*4+j] = 0.0;
		}

		// Ge�nderte Werte in die Modelviewmatrix �bertragen
		glLoadMatrixf(modelview);
}



GLvoid BillboardEnd() 
{

	// Zuvor gesicherte Matrix wieder vom Stack holen
	glPopMatrix();
}