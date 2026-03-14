//////////////////////////////////////////////////////////////////////////
// Funktionen für das View-Frustum
// siehe: 
//
// Fast Extraction of Viewing Frustum Planes from the World-
// View-Projection Matrix
// Gil Gribb,  Klaus Hartmann http://www2.ravensoft.com/users/ggribb/plane%20extraction.pdf
//
// und Frustum Culling
//     by Dion Picco (01 April 2003) (www.flipcode.com)

// Die folgende Funktion dient zur dynamischen Berücksichtigung der Ebenen (Panes) des 
//Viewfrustrums im aktuellen Sichtbereich (Field of View). Jeder Punkt auf diesen 
//Flächen lässt sich durch die Hessische Normalform beschreiben :
//
//HNF (P) = ax + by + cz + d = 0
//
//In dieser Darstellung ist eine Ebene durch einen Normalenvektor (x,y,z) und Abstand 
//vom Koordinatenursprung (d) definiert. Punkte, die auf der Seite der Ebene liegen,
//ergeben eingesetzt in die HNF einen positiven Wert, Punkte auf der anderen, nicht
//sichtbaren Seite einen negativen Wert.
//
//Zwar werden die Objekte dieser Punkte automatisch von OpenGL gecullt, die Information
//ob sie sichtbar sind oder nicht fehlt jedoch für andere Berechnungen. Diese Berechnung
//nuten wir, um einzelne Punkte/Objekte usw. gegen den Sichtbereich prüfen zu können.
//Liegen sie ausserhalb des Sichtbereiches, dann werden komplexe Berechnungen, wie z.B.
//die Kollisionsberechnung für Kamerafahrten ausgelassen um die Performance zu erhöhen.
//
//Damti diese Funktion genutzt werden kann, muss sie bei jeder Änderung der Kameraeigenschaften
//neu aufgerufen 
//werden, um die aktuelle Ausrichtung des FOV bestimmen zu können.


#include "stdafx.h"
#include "frustum.h"


//
// Konkatenieren der Modelview (m) Matrix mit der Projection (p) Matrix
// um eine Clippingmatrix im Model-Space zu erhalten
// Diese kann dann als Input für die Funktion ExtractPlanesGL() verwendet werden
// um das View-Frustum im Model-Space zu berechnen.
//
matrix4x4_t ClipMatrixModelSpace(float *m, float *p)
{
	matrix4x4_t	clip; // Clip-Matrix

	// Multiplizieren der MV- mit der P-Matrix
	// um die Frustum-Panes in Model-Space zu bekommen
	clip._11 = m[ 0] * p[ 0] + m[ 1] * p[ 4] + m[ 2] * p[ 8] + m[ 3] * p[12];
	clip._21 = m[ 0] * p[ 1] + m[ 1] * p[ 5] + m[ 2] * p[ 9] + m[ 3] * p[13];
	clip._31 = m[ 0] * p[ 2] + m[ 1] * p[ 6] + m[ 2] * p[10] + m[ 3] * p[14];
	clip._41 = m[ 0] * p[ 3] + m[ 1] * p[ 7] + m[ 2] * p[11] + m[ 3] * p[15];

	clip._12 = m[ 4] * p[ 0] + m[ 5] * p[ 4] + m[ 6] * p[ 8] + m[ 7] * p[12];
	clip._22 = m[ 4] * p[ 1] + m[ 5] * p[ 5] + m[ 6] * p[ 9] + m[ 7] * p[13];
	clip._32 = m[ 4] * p[ 2] + m[ 5] * p[ 6] + m[ 6] * p[10] + m[ 7] * p[14];
	clip._42 = m[ 4] * p[ 3] + m[ 5] * p[ 7] + m[ 6] * p[11] + m[ 7] * p[15];

	clip._13 = m[ 8] * p[ 0] + m[ 9] * p[ 4] + m[10] * p[ 8] + m[11] * p[12];
	clip._23 = m[ 8] * p[ 1] + m[ 9] * p[ 5] + m[10] * p[ 9] + m[11] * p[13];
	clip._33 = m[ 8] * p[ 2] + m[ 9] * p[ 6] + m[10] * p[10] + m[11] * p[14];
	clip._43 = m[ 8] * p[ 3] + m[ 9] * p[ 7] + m[10] * p[11] + m[11] * p[15];

	clip._14 = m[12] * p[ 0] + m[13] * p[ 4] + m[14] * p[ 8] + m[15] * p[12];
	clip._24 = m[12] * p[ 1] + m[13] * p[ 5] + m[14] * p[ 9] + m[15] * p[13];
	clip._34 = m[12] * p[ 2] + m[13] * p[ 6] + m[14] * p[10] + m[15] * p[14];
	clip._44 = m[12] * p[ 3] + m[13] * p[ 7] + m[14] * p[11] + m[15] * p[15];

	return clip;
}

// Ektrahieren der Clipping-Planes aus der übergebenen Matrix.
// Das normalize-Flag gibt an, ob die einzelnen Planes zu normalisieren sind
//
// 1. If the matrix M is equal to the projection matrix P (i.e., M = P), then the algorithm 
// gives the clipping planes in view space (i.e., camera space).
//
// 2. If the matrix M is equal to the combined projection and modelview matrices, then the algorithm 
// gives the clipping planes in model space (i.e.,  V * P = M, where V is the modelview matrix, and P 
// is  the projection matrix).
// Die Matrizen können wie folgt ermittelt werden:
//
//float	proj[16];       // PROJECTION Matrix
//float	modl[16];		// MODELVIEW Matrix
//glGetFloatv( GL_PROJECTION_MATRIX, proj );			
//glGetFloatv( GL_MODELVIEW_MATRIX, modl );	
//
//
// TODO: DIRTY-HACK erklären und beseitigen. Der Skalierungsfaktor wirkt sich nur auf
//		 das Far-Pane aus...
//
void ExtractPlanesGL(clipping_plane_t * p_planes,
					 const matrix4x4_t & comboMatrix,
					 bool normalize)
{
	// Left clipping plane
	p_planes[0].a = comboMatrix._41 + comboMatrix._11;
	p_planes[0].b = comboMatrix._42 + comboMatrix._12;
	p_planes[0].c = comboMatrix._43 + comboMatrix._13;
	p_planes[0].d = comboMatrix._44 + comboMatrix._14;
	// Right clipping plane
	p_planes[1].a = comboMatrix._41 - comboMatrix._11;
	p_planes[1].b = comboMatrix._42 - comboMatrix._12;
	p_planes[1].c = comboMatrix._43 - comboMatrix._13;
	p_planes[1].d = comboMatrix._44 - comboMatrix._14;
	// Top clipping plane
	p_planes[2].a = comboMatrix._41 - comboMatrix._21;
	p_planes[2].b = comboMatrix._42 - comboMatrix._22;
	p_planes[2].c = comboMatrix._43 - comboMatrix._23;
	p_planes[2].d = comboMatrix._44 - comboMatrix._24;
	// Bottom clipping plane
	p_planes[3].a = comboMatrix._41 + comboMatrix._21;
	p_planes[3].b = comboMatrix._42 + comboMatrix._22;
	p_planes[3].c = comboMatrix._43 + comboMatrix._23;
	p_planes[3].d = comboMatrix._44 + comboMatrix._24;
	// Near clipping plane
	p_planes[4].a = comboMatrix._41 + comboMatrix._31;
	p_planes[4].b = comboMatrix._42 + comboMatrix._32;
	p_planes[4].c = comboMatrix._43 + comboMatrix._33;
	p_planes[4].d = comboMatrix._44 + comboMatrix._34;
	// Far clipping plane
	p_planes[5].a = comboMatrix._41 - comboMatrix._31 / 20; // DIRTY-HACK: Skalierung rückgängig machen
	p_planes[5].b = comboMatrix._42 - comboMatrix._32/ 20;
	p_planes[5].c = comboMatrix._43 - comboMatrix._33/ 20;
	p_planes[5].d = comboMatrix._44 - comboMatrix._34/ 20;

	//devconout("a=%4.2f, b=%4.2f, c=%4.2f, d=%4.2f\n", p_planes[5].a, p_planes[5].b,p_planes[5].c,p_planes[5].d);  

	// Normalize the plane equations, if requested
	// Dies ist eine Änderung ggü. dem Originaldokument
	// Dort wurden 6 Funktionsaufrufe durchgeführt um die
	// Ebenen zu normalisieren. Da das inperformant ist wurde dies hier
	// geändert
	if (normalize == true)
	{
		float mag;

		for(int i = 0; i < 6; i++)
		{
			mag = sqrtf(p_planes[i].a * p_planes[i].a + 
						p_planes[i].b * p_planes[i].b +
						p_planes[i].c * p_planes[i].c);
			p_planes[i].a = p_planes[i].a / mag;
			p_planes[i].b = p_planes[i].b / mag;
			p_planes[i].c = p_planes[i].c / mag;
			p_planes[i].d = p_planes[i].d / mag;
		}

	}
}

//
//if the plane equation is not normalized, then we can still get some valuable information
//from the 'non-true' distance dist:
//1. If dist < 0 , then the point p lies in the negative halfspace (außerhalb).
//2. If dist = 0 , then the point p lies in the plane (auf).
//3. If dist > 0 , then the point p lies in the positive halfspace (innerhalb).
//
//This gives us another useful function that also works for non-normalized plane equations:
//
// Ermitteln ob ein Punkt pt auf, davor oder hinter einem Plane ist (entsprechend
// der Hessischen-Normalform
halfspace_t ClassifyPoint(const clipping_plane_t & plane, const vector3f_t & pt)
{
	float d = plane.a*pt.x + plane.b*pt.y + plane.c*pt.z + plane.d;
	
	halfspace_t hs;

	// elegantes if/then/else
	(d < 0) ? hs = NEGATIVE : (d > 0) ? hs = POSITIVE : hs = ON_PLANE;

	return hs;

	// Originalcode:
	//if (d < 0) return NEGATIVE;	// außerhalb
	//if (d > 0) return POSITIVE; // innerhalb
	//return ON_PLANE;			// darauf
}

// Gibt true zurück, wenn der angegebene Punkt im Frustum ODER auf einem Frustum-Plane
// liegt
bool isPointVisibleOnPlane(const clipping_plane_t *planes, const vector3f_t & pt)
{
	assert(planes && "isPointVisibleOnPlane: pointer to clipping planes not initialized.");
	
	halfspace_t hs;

	// Prüfen ob der Punkt innerhalb des Frustums liegt
	// 0 = left, 1 = right, 2 = top, 3 = bottom, 4 = near, 5 = far

	for(int i = 0 ; i < 6; i++)
	{
		// Punkt klassifizieren. Wenn außerhalb, dann ist er im negativen Halbraum
		hs = ClassifyPoint(planes[i], pt);
		// Sobald sich ergibt, dass der Punkt außerhalb auch nur eines Planes
		// liegt, kann die Prüfung sofort beendet werden
		if(hs == NEGATIVE)
			return false;
	} //for

	return true;
}

//
// Berechnung des aktuellen View-Frustums im Modelspace (MS). Dies muss geschehen,
// wenn sich die Modelviewmatrix oder die Projektionsmatrix ändert.
// Es wird die globale Variable mspace_planes[6] mit den aktuellen Frustum Werten gefüllt
//
//
// Gibt die neuen Frustum-Planes über den clipping_plane_t Zeiger zurück
void GetFrustumPanesMS(clipping_plane_t *planes)
{
	// Zeiger prüfen
	assert(planes && "GetFrustumPanesMS: invalid pointer to clipping_plane structure.");
	
	//////////////////////////////////////////////////////////////////////////
	// Frustum Culling
	// TODO: Muss nur erfolgen, wenn eine Änderung der M oder P Matrix erfolgt
	//		 (u.a. bei Kamerabewegung, aber auch manuell bei Skalieren/Translieren)
	//////////////////////////////////////////////////////////////////////////

	float	proj[16];       // PROJECTION Matrix
	float	modl[16];		// MODELVIEW Matrix
	matrix4x4_t	clip;		// Ergebnis der Konkatenation beider Matrizen

	// Aktuelle Modelview- und Projectionmatrix ermitteln
	glGetFloatv( GL_PROJECTION_MATRIX, proj );			
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );	

	// Clipping-Matrix im Modelspace ermitteln
	clip = ClipMatrixModelSpace(modl, proj);
	// Frustum-Panes ermitteln
	ExtractPlanesGL(planes, clip, true);
	//	setFrustum(planes);

	//	CalculateFrustum(modl, proj);
}

//////////////////////////////////////////////////////////////////////////
// TEST
//////////////////////////////////////////////////////////////////////////
/*

clipping_plane_t Frustum[6];

void CalculateFrustum(float *md, float *proj)
{
	// Error checking.
	assert(md != NULL || proj != NULL);

	// Create the clip.
	float clip[16] = {0};

	clip[0] = md[0] * proj[0] + md[1] * proj[4] + md[2] * proj[8]  + md[3] * proj[12];
	clip[1] = md[0] * proj[1] + md[1] * proj[5] + md[2] * proj[9]  + md[3] * proj[13];
	clip[2] = md[0] * proj[2] + md[1] * proj[6] + md[2] * proj[10] + md[3] * proj[14];
	clip[3] = md[0] * proj[3] + md[1] * proj[7] + md[2] * proj[11] + md[3] * proj[15];

	clip[4] = md[4] * proj[0] + md[5] * proj[4] + md[6] * proj[8]  + md[7] * proj[12];
	clip[5] = md[4] * proj[1] + md[5] * proj[5] + md[6] * proj[9]  + md[7] * proj[13];
	clip[6] = md[4] * proj[2] + md[5] * proj[6] + md[6] * proj[10] + md[7] * proj[14];
	clip[7] = md[4] * proj[3] + md[5] * proj[7] + md[6] * proj[11] + md[7] * proj[15];

	clip[8]  = md[8] * proj[0] + md[9] * proj[4] + md[10] * proj[8]  + md[11] * proj[12];
	clip[9]  = md[8] * proj[1] + md[9] * proj[5] + md[10] * proj[9]  + md[11] * proj[13];
	clip[10] = md[8] * proj[2] + md[9] * proj[6] + md[10] * proj[10] + md[11] * proj[14];
	clip[11] = md[8] * proj[3] + md[9] * proj[7] + md[10] * proj[11] + md[11] * proj[15];

	clip[12] = md[12] * proj[0] + md[13] * proj[4] + md[14] * proj[8]  + md[15] * proj[12];
	clip[13] = md[12] * proj[1] + md[13] * proj[5] + md[14] * proj[9]  + md[15] * proj[13];
	clip[14] = md[12] * proj[2] + md[13] * proj[6] + md[14] * proj[10] + md[15] * proj[14];
	clip[15] = md[12] * proj[3] + md[13] * proj[7] + md[14] * proj[11] + md[15] * proj[15];


	// Calculate the right side of the frustum.
	Frustum[0].a = clip[3]  - clip[0];
	Frustum[0].b = clip[7]  - clip[4];
	Frustum[0].c = clip[11] - clip[8];
	Frustum[0].d = clip[15] - clip[12];

	// Calculate the left side of the frustum.
	Frustum[1].a = clip[3]  + clip[0];
	Frustum[1].b = clip[7]  + clip[4];
	Frustum[1].c = clip[11] + clip[8];
	Frustum[1].d = clip[15] + clip[12];

	// Calculate the bottom side of the frustum.
	Frustum[2].a = clip[3]  + clip[1];
	Frustum[2].b = clip[7]  + clip[5];
	Frustum[2].c = clip[11] + clip[9];
	Frustum[2].d = clip[15] + clip[13];

	// Calculate the top side of the frustum.
	Frustum[3].a = clip[3]  - clip[1];
	Frustum[3].b = clip[7]  - clip[5];
	Frustum[3].c = clip[11] - clip[9];
	Frustum[3].d = clip[15] - clip[13];

	// Calculate the far side of the frustum.
	Frustum[4].a = clip[3]  - clip[2] /20;  // DIRTY-HACK...
	Frustum[4].b = clip[7]  - clip[6] /20;
	Frustum[4].c = clip[11] - clip[10] /20;
	Frustum[4].d = clip[15] - clip[14] /20;

	// Calculate the near side of the frustum.
	Frustum[5].a = clip[3]  + clip[2];
	Frustum[5].b = clip[7]  + clip[6];
	Frustum[5].c = clip[11] + clip[10];
	Frustum[5].d = clip[15] + clip[14];

	// Normalize the sides of the frustum.
	NormalizeFrustum();
}

void NormalizeFrustum()
{
	float magnitude = 0.0f;

	// Loop through each side of the frustum and normalize it.
	for(int i = 0; i < 6; i++)
	{
		magnitude = (float)sqrtf(Frustum[i].a * Frustum[i].a + 
			Frustum[i].b * Frustum[i].b + 
			Frustum[i].c * Frustum[i].c);
		magnitude = 1 / magnitude;

		Frustum[i].a *= magnitude;
		Frustum[i].b *= magnitude;
		Frustum[i].c *= magnitude;
		Frustum[i].d *= magnitude;
	}
}

bool isPointVisible(float x, float y, float z)
{
	// Loop through each side of the frustum and test if the point lies outside any of them.
	for(int i = 0; i < 6; i++)
	{
		if(Frustum[i].a * x + Frustum[i].b * y + Frustum[i].c * z + Frustum[i].d <= 0)
			return false;
	}

	return true;
}

void setFrustum(clipping_plane_t *p_planes)
{
	for(int i = 0; i<6;i++)
		Frustum[i] = p_planes[i];
	
}


*/