//////////////////////////////////////////////////////////////////////////
// Objekt-Informationen die einen Kubus betreffen

#ifndef _cube_t_H
#define _cube_t_H


// Struktur, die dazu dient, über Keyboard-Handler verändert zu werden
// um eine Steuerung zu ermöglichen. Weiterhin enthält diese Struktur
// Informationen über einen Cubus, 
typedef struct 
{
	GLfloat rotAlpha;		// Rotationswinkel
	GLfloat pos_x;			// Position x-Achse
	GLfloat pos_y;			
	GLfloat pos_z;
	GLboolean isVisible;
} cube_t;

cube_t *Initcube(GLvoid);
// cube_t in einer Display-Liste kompilieren
GLuint CompileCube(cube_t *cubeInformation, GLuint textureID);
// cube_t direkt rendern
GLvoid DrawCube(cube_t *cubeInformation, GLuint textureID);

#endif /*_cube_t_H */

