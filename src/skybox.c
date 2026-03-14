//////////////////////////////////////////////////////////////////////////
// Skybox Funktionen

#include "stdafx.h"
#include "skybox.h"
#include "texture.h"		// Für die Konstante GL_CLAMP_TO_EDGE

//
// NAME:	DrawSkyBox
//
// ZWECK: Zeichnet eine Skybox mit den angegebenen Textur-IDs
//
void DrawSkyBox(GLfloat x, GLfloat y, GLfloat z,				// Zentrum der Skybox
				GLfloat width, GLfloat height, GLfloat depth,	// Gesamtausdehnung der Box auf den Achsen 
				GLuint aheadId, GLuint backId, GLuint downId,	// 6 Textur-IDs für die Box 
				GLuint leftId, GLuint rightId, GLuint upId)
{
	//TODO: Displayliste zum Optimieren!
	
	// Turn on texture mapping if it's not already
	glEnable(GL_TEXTURE_2D);

	// Bind the BACK texture of the sky map to the BACK side of the cube_t
	glBindTexture(GL_TEXTURE_2D, backId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// This centers the sky box around (x, y, z)
	x = x - width  / 2;
	y = y - height / 2;
	z = z - depth / 2;

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		

	// Assign the texture coordinates and vertices for the BACK Side
	glTexCoord2f(1.0f, 0.0f); profile_glVertex3f(x + width, y,			z);
	glTexCoord2f(1.0f, 1.0f); profile_glVertex3f(x + width, y + height, z); 
	glTexCoord2f(0.0f, 1.0f); profile_glVertex3f(x,			y + height, z);
	glTexCoord2f(0.0f, 0.0f); profile_glVertex3f(x,			y,			z);

	glEnd();

	// Bind the FRONT texture of the sky map to the FRONT side of the box
	glBindTexture(GL_TEXTURE_2D, aheadId );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);	

	// Assign the texture coordinates and vertices for the FRONT Side
	glTexCoord2f(1.0f, 0.0f); profile_glVertex3f(x,			y,			z + depth);
	glTexCoord2f(1.0f, 1.0f); profile_glVertex3f(x,			y + height, z + depth);
	glTexCoord2f(0.0f, 1.0f); profile_glVertex3f(x + width, y + height, z + depth); 
	glTexCoord2f(0.0f, 0.0f); profile_glVertex3f(x + width, y,			z + depth);
	glEnd();

	// Bind the BOTTOM texture of the sky map to the BOTTOM side of the box
	glBindTexture(GL_TEXTURE_2D, downId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		

	// Assign the texture coordinates and vertices for the BOTTOM Side
	glTexCoord2f(1.0f, 0.0f); profile_glVertex3f(x,			y,			z);
	glTexCoord2f(1.0f, 1.0f); profile_glVertex3f(x,			y,			z + depth);
	glTexCoord2f(0.0f, 1.0f); profile_glVertex3f(x + width, y,			z + depth); 
	glTexCoord2f(0.0f, 0.0f); profile_glVertex3f(x + width, y,			z);
	glEnd();

	// Bind the TOP texture of the sky map to the TOP side of the box
	glBindTexture(GL_TEXTURE_2D, upId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		

	// Assign the texture coordinates and vertices for the TOP Side
	glTexCoord2f(0.0f, 1.0f); profile_glVertex3f(x + width, y + height, z);
	glTexCoord2f(0.0f, 0.0f); profile_glVertex3f(x + width, y + height, z + depth); 
	glTexCoord2f(1.0f, 0.0f); profile_glVertex3f(x,			y + height,	z + depth);
	glTexCoord2f(1.0f, 1.0f); profile_glVertex3f(x,			y + height,	z);

	glEnd();

	// TODO: Überlegen, warum links und rechts vertauscht sind

	// Bind the LEFT texture of the sky map to the LEFT side of the box
	glBindTexture(GL_TEXTURE_2D, rightId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		

	// Assign the texture coordinates and vertices for the LEFT Side
	glTexCoord2f(1.0f, 1.0f); profile_glVertex3f(x,			y + height,	z);	
	glTexCoord2f(0.0f, 1.0f); profile_glVertex3f(x,			y + height,	z + depth); 
	glTexCoord2f(0.0f, 0.0f); profile_glVertex3f(x,			y,			z + depth);
	glTexCoord2f(1.0f, 0.0f); profile_glVertex3f(x,			y,			z);		

	glEnd();

	// Bind the RIGHT texture of the sky map to the RIGHT side of the box
	glBindTexture(GL_TEXTURE_2D, leftId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		

	// Assign the texture coordinates and vertices for the RIGHT Side
	glTexCoord2f(0.0f, 0.0f); profile_glVertex3f(x + width, y,			z);
	glTexCoord2f(1.0f, 0.0f); profile_glVertex3f(x + width, y,			z + depth);
	glTexCoord2f(1.0f, 1.0f); profile_glVertex3f(x + width, y + height,	z + depth); 
	glTexCoord2f(0.0f, 1.0f); profile_glVertex3f(x + width, y + height,	z);
	glEnd();

	glDisable(GL_TEXTURE_2D);

}