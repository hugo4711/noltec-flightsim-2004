//////////////////////////////////////////////////////////////////////////
// 2Dfonts.h

#ifndef _2DFONTS_H
#define _2DFONTS_H


// Maximale Anzahl der zu erzeugenden Zeichen (ASCII-Code)
#define MAX_CHAR		256
#define FONT_HEIGHT		14

// Erzeugt MAX_CHAR viele Display-Listen (für jedes Zeichen eine)
GLuint CreateGLFontList(HDC		hDC,
						LPCSTR	strFontName, 
						UINT	nHeight);

// 2D-Text auf dem Bildschirm ausgeben
void glDrawText2D(GLuint fontListID,
				  int viewport_height,
				  int x,					
				  int y,					
				  const char *strString,	
				  ...);	

// Positionieren des 2D-Textes (wird von glDrawText2D gerufen)
void PositionText2D( int x, int y, int viewport_height );

#endif /* _2DFONTS_H */