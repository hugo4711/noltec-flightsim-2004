//////////////////////////////////////////////////////////////////////////
// Funktionen, die den Würfel betreffen
#include "stdafx.h"
#include "cube.h"

//
// NAME: Initcube_t
//
// ZWECK: Cubus initialisieren
cube_t  *Initcube() {
	// Cubus-Informationen generieren und initialisieren
	cube_t *cubeInformation = (cube_t *) calloc(1, sizeof(cube_t));
	if(!cubeInformation) {
		MessageBox(NULL, "Not enough memory error. (cubeInformation)", "Error", MB_OK | MB_ICONEXCLAMATION);
		// Beenden
		PostQuitMessage(-1);
	}
	// Intitialisieren
	cubeInformation->rotAlpha = 0;
	cubeInformation->pos_x = 0;
	cubeInformation->pos_y = 0;
	cubeInformation->pos_z = 0;
	cubeInformation->isVisible = GL_TRUE;

	return cubeInformation;
}

//
// NAME: Compilecube()
//
// ZWECK: cube_t in einer Display-Liste kompilieren
//
// KOMMENTAR: Gibt einen eindeutigen Namen für die Liste zurück
GLuint CompileCube (cube_t *cubeInformation, GLuint textureID) 
{
	// glGenLists(N) erzeugt N viele Listeneinträge und gibt einen Bezeichner
	// (base-pointer als GLuint) über den die Liste angesprochen werden kann, zurück.
	// Da hier nur ein Objekt gezeichnet wird, benötigen wir 1 Eintrag.
	GLuint listname = glGenLists(1);
	// Liste erzeugen und kompilieren
	glNewList(listname, GL_COMPILE);
		DrawCube(cubeInformation, textureID);
	glEndList();
	// Testen, ob die Display-Liste existiert
	assert(  glIsList(listname) == GL_TRUE);
	
	return listname;
}

//
//  NAME: Drawcube)
//
//  ZWECK: Würfel zeichnen, mit der geladenen Textur
//
GLvoid DrawCube(cube_t *cubeInformation,		// Information über den Würfel zum Manipulieren
					GLuint textureID)			// Textur die darüber gemappt werden soll
{	

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, textureID);
	
	// vorne	
	glBegin(GL_QUADS);
	//glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, -1.0f);  
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, -1.0f);  
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);  
	 glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 1.0f, -1.0f); 
	glEnd();
	
	
	// hinten
	//glColor3f(0.0f, 0.0f, 1.0f);
	//glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	//glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);  
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);  
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);  
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);  
	glEnd();

	// unten
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
		//glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);  
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);  
		 glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 0.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, -1.0f); 
	glEnd();

	// oben								
	//glColor3f(0.0f, 1.0f, 0.0f);
	//glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
		//glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);  
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);  
		 glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 1.0f, -1.0f); 
	glEnd();


	// rechts
	//glColor3f(1.0f, 1.0f, 1.0f);
	//glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
		//glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);  
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, -1.0f); 
		 glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);  
	glEnd();

	// links
	//glColor3f(0.5f, 0.5f, 1.0f);
	//glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
		//glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);   
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f, -1.0f);  
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, 1.0f, -1.0f);  
		 glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);  
	glEnd(); 

	glDisable(GL_TEXTURE_2D);
}

