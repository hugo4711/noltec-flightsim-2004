//////////////////////////////////////////////////////////////////////////
// Texturloader - loader.h
#ifndef _LOADER_H
#define _LOADER_H

#include "TGAfilespec.h"		// TGA-Dateispezifikation einbinden
#include "texture.h"			// Texturdefinitionen


// Bildstruktur (kompatibel zu glaux AUX_RGBImageRec)
typedef struct _AUX_RGBImageRec {
	GLint sizeX, sizeY;
	unsigned char *data;
} AUX_RGBImageRec;

// Texturen werden ber diese Funktion geladen
int	LoadTexture(LPCSTR lpTexturename,		// Dateiname der Textur
		  	    ldr_texturefmt_t type,	// Typ der Textur: BMP, TGA
			    GLuint *textureID,		// Ziel f�r die Textur-ID
				GLboolean mipmapped);			// mipmapping Flag

// L�dt ein Bitmap in den Speicher, basierend auf Auxilary Lib
AUX_RGBImageRec *LoadBMP(LPCSTR filename);

// TGA-Loader Entry
tgaimagerec_t *LoadTGA(LPCSTR filename);
// TGA Interner Loader f�r komprimierte Files (wird von LoadTGA gerufen
GLboolean LoadCompressedTGA(tgaimagerec_t *imgrec, FILE *fTGA);
// TGA Interner Loader f�r unkomprimierte TGAs
GLboolean LoadUncompressedTGA(tgaimagerec_t *imgrec, FILE * fTGA);

// RAW-File mit bin�ren Daten laden ("rudiment�rstes" Format)
int LoadRAWFile(LPCSTR strName, GLuint nSize, BYTE *pHeightMap);

#endif /* LOADER_H */