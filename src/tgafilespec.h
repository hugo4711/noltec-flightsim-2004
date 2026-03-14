//////////////////////////////////////////////////////////////////////////
// Definitionen, die das TGA-Dateiformat spezifizieren

#ifndef _TGAFILESPEC_H
#define _TGAFILESPEC_H


// Header des TGA-Files. Anhand des Headers wird überprüft 
// ob das TGA compressed oder uncompressed ist
typedef struct
{
	GLubyte header[12];
} tgaheader_t;

// Definitionen von zwei Variablen, die dem Vergleich dienen und
// jeweils die Bytefolge für compressed/uncompressed TGA angeben
//const GLubyte tgaUncompressed [12] = {0,0, 2,0,0,0,0,0,0,0,0,0};
//const GLubyte tgaCompressed	[12] = {0,0,10,0,0,0,0,0,0,0,0,0};

#define TGA_UNCOMPRESSED {0,0, 2,0,0,0,0,0,0,0,0,0}		
#define TGA_COMPRESSED	 {0,0,10,0,0,0,0,0,0,0,0,0}

// TGA-Bildinformationen
typedef struct
{
	GLubyte header[6];
	GLuint	bytesPerPixel;
	GLuint	imageSize;
	GLuint	type;             //Der Typ der Grafik
	GLuint	height;
	GLuint	width;
	GLuint	bpp;             //Anzahl der Bits per Pixel
} tagfileinfo_t;

// Bildstruktur (ähnlich AUX_RGBImageRec)
typedef struct
{
	GLubyte*	data;			//Zeiger auf sämtliche Farbwerte der Textur (was: imageData)
	GLuint		bpp;			//Speichervariable für die Bitanzahl 
	GLuint		sizeX;			//Speichervariable für Texturbreite		(was: width)
	GLuint		sizeY;			//Speichervariable für Texturhöhe		(was: height)
	//GLuint		texID;          //Texture ID (nicht benötigt!)
	GLuint		type;			
} tgaimagerec_t;


#endif /* _TGAFILESPEC_H */