//////////////////////////////////////////////////////////////////////////
// Skybox Header File

#ifndef _SKYBOX_H
#define _SKYBOX_H

// Zeichnet eine Skybox um den Punkt mit der angegebenen Ausdehnung und den Texturen
void DrawSkyBox(GLfloat x, GLfloat y, GLfloat z,				// Zentrum der Skybox
				GLfloat width, GLfloat height, GLfloat depth,	// Gesamtausdehnung der Box auf den Achsen 
				GLuint aheadId, GLuint backId, GLuint downId,	// 6 Textur-IDs für die Box 
				GLuint leftId, GLuint rightId, GLuint upId);

#endif /* _SKYBOX_H */