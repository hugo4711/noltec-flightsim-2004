// Texture-Loader

#include "stdafx.h"
#include "loader.h"

//
//	NAME: LoadBMP
//
//	ZWECK: L�dt Device Independent Bitmaps in den Speicher
//
//	KOMMENTAR: Einfacher Bitmap Loader, verwendet die Auxilary Lib
//
AUX_RGBImageRec *LoadBMP(LPCSTR lpFilename) {
	FILE *hFile = fopen(lpFilename, "rb");
	if (!hFile) return NULL;

	BITMAPFILEHEADER bmfh;
	fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, hFile);
	if (bmfh.bfType != 0x4D42) {
		fclose(hFile);
		return NULL;
	}

	BITMAPINFOHEADER bmih;
	fread(&bmih, sizeof(BITMAPINFOHEADER), 1, hFile);

	fseek(hFile, bmfh.bfOffBits, SEEK_SET);

	AUX_RGBImageRec *image = (AUX_RGBImageRec *)malloc(sizeof(AUX_RGBImageRec));
	image->sizeX = bmih.biWidth;
	image->sizeY = abs(bmih.biHeight);
	int imageSize = image->sizeX * image->sizeY * 3;
	image->data = (unsigned char *)malloc(imageSize);

	fread(image->data, imageSize, 1, hFile);
	fclose(hFile);

	// BGR -> RGB swap
	for (int i = 0; i < imageSize; i += 3) {
		unsigned char temp = image->data[i];
		image->data[i] = image->data[i + 2];
		image->data[i + 2] = temp;
	}

	return image;
}



//
// NAME: LoadTexture()
//
// ZWECK: Laden s�mtlicher Texturen
//
// KOMMENTAR: L�dt eine Textur in den Speicher, fordert eine Textur-ID an und
//			  schreibt diese in textureID
//			  Das Laden der Texturen sollte beim Initialisieren der GL-Szene geschehen.
//
// R�CKGABEWERTE:
//			-1 : Speicherallokation schlug fehl
//			-2 : Textur konnte nicht geladen werden
//			-3 : Typ nicht unterst�tzt
//			-4 : RAW-Files m�ssen �ber LoadRAWFile geladen werden
//			ansonsten 0

int LoadTexture(LPCSTR lpTexturename,		// Dateiname der Textur
				ldr_texturefmt_t type,		// Typ der Textur (Enumeration: BMP, TGA)
				GLuint *textureID,			// Zeiger auf die Var. die die Textur-ID erh�lt
				GLboolean mipmapped)				// wenn wahr, dann wird mipmapping angewandt
{
	AUX_RGBImageRec *bmpTextureImage;	// Zeiger f�r die BMP-Struktur
	tgaimagerec_t	*tgaTextureImage;   // Zeiger f�r die TGA-Struktur

	GLenum minFilter, maxFilter;		// Filter die auf die Textur anzuwenden sind

	// �berpr�fen ob die �bergebenen Typen hier auch implementiert sind
	assert((type == FMT_BMP || type == FMT_TGA || type == FMT_RAW) &&
			"LoadTexture: invalid texture type specified.");
	
	// Wenn Mipmapping angewendet werden soll, dann wird der stauchen Filter auf
	// Mipmapping eingestellt. Ansonsten findet lineares Filtering statt
	if(mipmapped == GL_TRUE)
	{
		minFilter = GL_LINEAR_MIPMAP_LINEAR;
		maxFilter = GL_LINEAR;
	} else 
	{
		minFilter = GL_LINEAR;
		maxFilter = GL_LINEAR;
	}


	//////////////////////////////////////////////////////////////////////////
	// BMP laden
	//////////////////////////////////////////////////////////////////////////
	
	if(type == FMT_BMP) {
		// Speicher f�r die BMP-Textur allokieren
		//bmpTextureImage = (AUX_RGBImageRec *) malloc(sizeof(AUX_RGBImageRec));

		//if(!bmpTextureImage)
		//	return -1;	// Fehler beim Allokieren von Speicher

		// Laden der BMP - Textur
		bmpTextureImage = LoadBMP(lpTexturename);

		if(!bmpTextureImage)
			return -2; // Fehler beim Laden der Textur
	
		// Eindeutiger Texturbezeichner wird erzeugt
		glGenTextures(1, textureID); 
		// binds the texture name (textureID) to the appropriate texture target (GL_TEXTURE_2D)
		glBindTexture(GL_TEXTURE_2D, *textureID);

		// Texturfilterung: Was passiert, wenn die Texturwerte von Polygonwerte abweichen?
		glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			minFilter
			/* GL_LINEAR_MIPMAP_NEAREST */); // Strecken, linearer Algorithmus
		glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			maxFilter
			/* GL_LINEAR_MIPMAP_NEAREST */); // Stauchen

		// Spezielle Mipmapfunktion, die die entsprechenden Mipmaps bis auf 1 Texel herunterrechnet
		
		if(mipmapped == GL_TRUE)
			gluBuild2DMipmaps(GL_TEXTURE_2D,
				3,
				bmpTextureImage->sizeX,
				bmpTextureImage->sizeY,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				bmpTextureImage->data);
		else
			// Normale 2D-Texturfunktion
			glTexImage2D(GL_TEXTURE_2D,
				0,				// Aufl�sung der Textur
				GL_RGB,		// Internes Texturformat
				bmpTextureImage->sizeX,
				bmpTextureImage->sizeY,
				0,				// Rahmen um die Textur zeichnen? 0 = nein
				GL_RGB,		// Texturformat
				GL_UNSIGNED_BYTE,// Typ der Texturdaten
				bmpTextureImage->data);

		// Speicherbereinigung
		//free(bmpTextureImage->data); 
		//free(bmpTextureImage);
		
		// Zur�ck, kein Fehler
		return 0;
		
	} // if( type == FMT_BMP)
	

	//////////////////////////////////////////////////////////////////////////
	// TGA laden
	//////////////////////////////////////////////////////////////////////////


	if (type == FMT_TGA)
	{
		// TODO: ERROR Handling beim TGA laden

		//tgaTextureImage = (tgaimagerec_t *) malloc(sizeof(tgaimagerec_t));
		//if(!tgaTextureImage) 
		//	return -1;			// nicht gen�gend Speicher

		tgaTextureImage = LoadTGA(lpTexturename);
		if(!tgaTextureImage)
			return -2;			// Fehler beim Laden der Textur
			
		glGenTextures(1, textureID);
		glBindTexture(GL_TEXTURE_2D, *textureID);

		// Setzen der Mipmap Filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter /*GL_LINEAR_MIPMAP_NEAREST*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter /*GL_LINEAR*/);

		/* Zuweisen der Grafik, hier muss darauf geachtet werden, dass wir einen Alpha-Kanal unter-
		st�tzen, daher muss hier der 2. Parameter 4 lauten und GL_RGBA gesetzt werden ! */
		if(mipmapped)
			gluBuild2DMipmaps(GL_TEXTURE_2D,
							4,
							tgaTextureImage->sizeX,
							tgaTextureImage->sizeY,
							GL_RGBA,
							GL_UNSIGNED_BYTE,
							tgaTextureImage->data);		
		else
			// Normale 2D-Texturfunktion
			glTexImage2D(GL_TEXTURE_2D,
				0,				// Aufl�sung der Textur
				GL_RGBA,		// Internes Texturformat
				tgaTextureImage->sizeX,
				tgaTextureImage->sizeY,
				0,				// Rahmen um die Textur zeichnen? 0 = nein
				GL_RGBA,		// Texturformat
				GL_UNSIGNED_BYTE,// Typ der Texturdaten
				tgaTextureImage->data);
		
		// TODO: Hier ist irgendwo noch ein BUG versteckt. Beim Laden einer 512x512 24 Bit
		//		 uncompressed Datei. Trat hier eine unbehandelte Ausnahme auf!

		free(tgaTextureImage->data);
		free(tgaTextureImage);
		
		// Kein Fehler bis hierher
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// RAW Dateiformat
	//////////////////////////////////////////////////////////////////////////
	
	if(type == FMT_RAW) 
	{
		return -4;
	}
	
	return -3; // Typ nicht unterst�tzt

}


//
// NAME: LoadTGA
//
// ZWECK: TGA komprimiert oder unkomprimiert laden
//
tgaimagerec_t *LoadTGA(LPCSTR filename)
{
	tgaimagerec_t *imgrec;	// TGA-Filestruktur
	FILE * fTGA;			// TGA-Dateizeiger
	tgaheader_t tgaheader_t;	// TGA-Fileheader

	GLubyte tgaUncompressed [12] = TGA_UNCOMPRESSED;
	GLubyte tgaCompressed	[12] = TGA_COMPRESSED;

	imgrec = (tgaimagerec_t *) malloc(sizeof(tgaimagerec_t));
	if(!imgrec)
		return NULL;

	// TGA-File �ffnen im Bin�r-Mode
	fTGA = fopen(filename, "rb");
	if(!fTGA)
		return NULL;

	// File-Header einlesen
	if(fread(&tgaheader_t, sizeof(tgaheader_t), 1, fTGA) == 0)
	{
		fclose(fTGA); free(imgrec);	
		return NULL;
	}

	// Header pr�fen ob unkomprimiert oder komprimiert 
	if(memcmp(tgaUncompressed, &tgaheader_t, sizeof(tgaheader_t))==0)
	{
		// Laden eines unkomprimiertes TGA-File
		if(!LoadUncompressedTGA(imgrec, fTGA) )
			return NULL;
	}
	else if(memcmp(tgaCompressed, &tgaheader_t, sizeof(tgaheader_t)) == 0)
	{
		// Laden eines komprimierten TGA-File
		if(!LoadCompressedTGA(imgrec, fTGA))
			return NULL;
	}
	else
	{
		// unbekannter Dateityp
		fclose(fTGA);
		free(imgrec);
		return NULL;
	}
	return imgrec;
}

//
// NAME: LoadUncompressedTGA
// 
// ZWECK: Unkomprimierte TGA-Files laden
//
// KOMMENTAR: Gibt GL_FALSE zur�ck, wenn etwas schiefl�uft

GLboolean LoadUncompressedTGA(tgaimagerec_t *imgrec, FILE * fTGA)
{
	tagfileinfo_t tagfileinfo_t;
	GLuint cswap;
	
	// N�chste sechs Bytes Header-Infos laden
	if(fread(tagfileinfo_t.header, sizeof(tagfileinfo_t.header), 1, fTGA) == 0)
	{
		fclose(fTGA);
		return GL_FALSE;
	}

	/* Nun haben wir alle Informationen, die wir ben�tigen um die H�he, Breite und Bits pro Pixel f�r die Grafik
	zu ermitteln. Die ermittelten Werte speichern wir sowohl in der Struktur f�r die Textur als auch lokal. */

	imgrec->sizeX  = tagfileinfo_t.header[1] * 256 + tagfileinfo_t.header[0];	// Height
	imgrec->sizeY = tagfileinfo_t.header[3] * 256 + tagfileinfo_t.header[2];	// Width
	imgrec->bpp = tagfileinfo_t.header[4];							// Bits Per Pixel

	// Sichern der Bildinformationen in der lokalen Struktur
	tagfileinfo_t.width = imgrec->sizeX;   
	tagfileinfo_t.height = imgrec->sizeY;
	tagfileinfo_t.bpp  = imgrec->bpp;


	/* Als n�chstes pr�fen wir ob die H�he und Breite nicht mehr als ein Pixel betr�gt und ob die Bpp einen 24 
	oder 32 Bit Farbwert aufweisen. Kommt es hierbei zu einer Abweichung wird erneut abgebrochen */

	if( (imgrec->sizeX <= 0) || (imgrec->sizeY <= 0) ||
	  ( (imgrec->bpp != 24) && (imgrec->bpp !=32) ) )
	{
		fclose(fTGA);
		MessageBox(NULL, "Die Bildinformationen weichen von den g�ltigen Parametern ab!", "ERROR", MB_OK);
		return GL_FALSE;
	}

	/* In den n�chsten Schritten setzen wir den Farbwert der Grafiken, nur bei 32 Bit Farben unterst�tzen wir
	den Alphakanal */

	if(imgrec->bpp == 32)
	{
		imgrec->type= GL_RGBA;
	}
	else
	{
		imgrec->type= GL_RGB;
	}

	// Als n�chstes berechnen wir die Bytes per Pixel und die Gr��e der Grafikdatei
	tagfileinfo_t.bytesPerPixel = (tagfileinfo_t.bpp / 8); 
	tagfileinfo_t.imageSize = (tagfileinfo_t.bytesPerPixel * tagfileinfo_t.width * tagfileinfo_t.height);

	// Damit die Daten gespeichert werden k�nnen wird im Folgeschritt Speicher allokiert
	imgrec->data = (GLubyte *)malloc(tagfileinfo_t.imageSize);

	if(imgrec->data == NULL)
	{
		fclose(fTGA);
		MessageBox(NULL, "Speicherreservierung f�r Textur fehlgeschlagen!", "ERROR", MB_OK);
		return GL_FALSE;
	}

	/* Als n�chstes wird versucht die Datei anhand der ermittelten Daten zu laden, schl�gt dies Fehl, wird
	wie gehabt eine Fehlermeldung erzeugt. */

	if(fread(imgrec->data, 1, tagfileinfo_t.imageSize, fTGA) != tagfileinfo_t.imageSize)
	{
		fclose(fTGA);
		MessageBox(NULL, "Die Bilddaten konnten nicht geladen werden!", "ERROR", MB_OK);
		return GL_FALSE;
	}

	/* In TGA Dateien sind die Bildinformationen entgegengesetzt gespeichert. Daher m�ssen f�r OpenGL die BGR 
	Informationen in lesbare RGB Daten umgewandelt werden. Diese Umwandlung erzeugen wir anhand einer for-
	Schleife, bei der wir lediglich 3 Bin�roperationen auf XOR Basis durchf�hren */

	for(cswap = 0; cswap < (int)tagfileinfo_t.imageSize; cswap += tagfileinfo_t.bytesPerPixel)
	{
		// 1st Byte XOR 3rd Byte XOR 1st Byte XOR 3rd Byte
		imgrec->data[cswap] ^= imgrec->data[cswap+2] ^=
			imgrec->data[cswap] ^= imgrec->data[cswap+2];
	}

	//Anschlie�end schlie�en wir die Datei und geben eine Erfolgsmeldung zur�ck
	fclose(fTGA);
	return GL_TRUE;
}


//
// NAME: LoadCompressedTGA
//
// ZWECK: L�dt komprimierte TGA-Files in eine tgaimagerec_t-Struktur
//
// KOMMENTAR: Gibt GL_FALSE zur�ck, wenn irgendwo hier ein Fehler auftaucht
GLboolean LoadCompressedTGA(tgaimagerec_t *imgrec, FILE *fTGA)
{ 
	tagfileinfo_t tagfileinfo_t;	//Dateiinformations-Struktur
	
	// Variablen f�r die Bilddaten
	GLuint pixelcount = 0;										// Nuber of pixels in the image
	GLuint currentpixel	= 0;								// Current pixel being read
	GLuint currentbyte	= 0;								// Current byte 
	GLubyte *colorbuffer;
	// Laufz�hler
	short counter;
	
	// weitere 6 bytes aus der Datei auslesen
	// Das entspricht den weiteren Header-Daten (gr��e, bpp)
	if(fread(tagfileinfo_t.header, sizeof(tagfileinfo_t.header), 1, fTGA) == 0)					
	{
		fclose(fTGA);
		return GL_FALSE;														
	}

	// Ermitteln und Speichern der Bildinformationen
	imgrec->sizeX   = tagfileinfo_t.header[1] * 256 + tagfileinfo_t.header[0];  
	imgrec->sizeY   = tagfileinfo_t.header[3] * 256 + tagfileinfo_t.header[2];	
	imgrec->bpp  	= tagfileinfo_t.header[4];						

	// Speichern in der lokalen Struktur
	tagfileinfo_t.width		= imgrec->sizeX;									
	tagfileinfo_t.height		= imgrec->sizeY;									
	tagfileinfo_t.bpp			= imgrec->bpp;										

	// Pr�fen der Werte auf G�ltigkeit und Bit-Farbwert
	if( (imgrec->sizeX <= 0) || (imgrec->sizeY <= 0) || 
	    ( (imgrec->bpp != 24) && (imgrec->bpp !=32) ) )	
	{
		fclose(fTGA);
		return GL_FALSE;
	}

	// Ermitteln der Bytes per Pixel und Allokieren von Speicher
	tagfileinfo_t.bytesPerPixel	= (tagfileinfo_t.bpp / 8);									
	tagfileinfo_t.imageSize		= (tagfileinfo_t.bytesPerPixel * tagfileinfo_t.width * tagfileinfo_t.height);		
	imgrec->data	= (GLubyte *)malloc(tagfileinfo_t.imageSize);					

	// Sicherheits�berpr�fung der Speicherallokation
	if(imgrec->data == NULL)										
	{	
		fclose(fTGA);
		return GL_FALSE;	
	}


	//////////////////////////////////////////////////////////////////////////
	// Dekomprimieren des TGA-Files
	// Erst jetzt wird das Datenermittlungsverfahren f�r komprimierte TGA-Files 
	// abweichend durchgef�hrt.
	//////////////////////////////////////////////////////////////////////////
	
		
	pixelcount	= tagfileinfo_t.height * tagfileinfo_t.width;		// Nuber of pixels in the image
	colorbuffer = (GLubyte *)malloc(tagfileinfo_t.bytesPerPixel);	// Storage for 1 pixel

	do
	{
		GLubyte chunkheader = 0;											// Storage for "chunk" header

		if(fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0)				// Read in the 1 byte header
		{
			MessageBox(NULL, "Could not read RLE header", "ERROR", MB_OK);	// Display Error
			if(fTGA != NULL)												// If file is open
			{
				fclose(fTGA);												// Close file
			}
			if(imgrec->data != NULL)									// If there is stored image data
			{
				free(imgrec->data);									// Delete image data
			}
			return GL_FALSE;													// Return failed
		}
		if(chunkheader < 128)												// If the ehader is < 128, it means the that is the number of RAW color packets minus 1
		{																	// that follow the header
			chunkheader++;													// add 1 to get number of following color values
			for(counter = 0; counter < chunkheader; counter++)		// Read RAW color values
			{
				if(fread(colorbuffer, 1, tagfileinfo_t.bytesPerPixel, fTGA) != tagfileinfo_t.bytesPerPixel) // Try to read 1 pixel
				{
					MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);		// IF we cant, display an error

					if(fTGA != NULL)													// See if file is open
					{
						fclose(fTGA);													// If so, close file
					}

					if(colorbuffer != NULL)												// See if colorbuffer has data in it
					{
						free(colorbuffer);												// If so, delete it
					}

					if(imgrec->data != NULL)										// See if there is stored Image data
					{
						free(imgrec->data);										// If so, delete it too
					}

					return GL_FALSE;														// Return failed
				}
				// write to memory
				imgrec->data[currentbyte		] = colorbuffer[2];				    // Flip R and B vcolor values around in the process 
				imgrec->data[currentbyte + 1	] = colorbuffer[1];
				imgrec->data[currentbyte + 2	] = colorbuffer[0];

				if(tagfileinfo_t.bytesPerPixel == 4)												// if its a 32 bpp image
				{
					imgrec->data[currentbyte + 3] = colorbuffer[3];				// copy the 4th byte
				}

				currentbyte += tagfileinfo_t.bytesPerPixel;										// Increase thecurrent byte by the number of bytes per pixel
				currentpixel++;															// Increase current pixel by 1

				if(currentpixel > pixelcount)											// Make sure we havent read too many pixels
				{
					MessageBox(NULL, "Too many pixels read", "ERROR", NULL);			// if there is too many... Display an error!

					if(fTGA != NULL)													// If there is a file open
					{
						fclose(fTGA);													// Close file
					}	

					if(colorbuffer != NULL)												// If there is data in colorbuffer
					{
						free(colorbuffer);												// Delete it
					}

					if(imgrec->data != NULL)										// If there is Image data
					{
						free(imgrec->data);										// delete it
					}

					return GL_FALSE;														// Return failed
				}
			}
		}
		else																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
		{
			chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
			if(fread(colorbuffer, 1, tagfileinfo_t.bytesPerPixel, fTGA) != tagfileinfo_t.bytesPerPixel)		// Attempt to read following color values
			{	
				MessageBox(NULL, "Could not read from file", "ERROR", MB_OK);			// If attempt fails.. Display error (again)

				if(fTGA != NULL)														// If thereis a file open
				{
					fclose(fTGA);														// Close it
				}

				if(colorbuffer != NULL)													// If there is data in the colorbuffer
				{
					free(colorbuffer);													// delete it
				}

				if(imgrec->data != NULL)											// If thereis image data
				{
					free(imgrec->data);											// delete it
				}

				return GL_FALSE;															// return failed
			}

			for(counter = 0; counter < chunkheader; counter++)					// copy the color into the image data as many times as dictated 
			{																			// by the header
				imgrec->data[currentbyte		] = colorbuffer[2];					// switch R and B bytes areound while copying
				imgrec->data[currentbyte + 1	] = colorbuffer[1];
				imgrec->data[currentbyte + 2	] = colorbuffer[0];

				if(tagfileinfo_t.bytesPerPixel == 4)												// If TGA images is 32 bpp
				{
					imgrec->data[currentbyte + 3] = colorbuffer[3];				// Copy 4th byte
				}

				currentbyte += tagfileinfo_t.bytesPerPixel;										// Increase current byte by the number of bytes per pixel
				currentpixel++;															// Increase pixel count by 1

				if(currentpixel > pixelcount)											// Make sure we havent written too many pixels
				{
					MessageBox(NULL, "Too many pixels read", "ERROR", NULL);			// if there is too many... Display an error!

					if(fTGA != NULL)													// If there is a file open
					{
						fclose(fTGA);													// Close file
					}	

					if(colorbuffer != NULL)												// If there is data in colorbuffer
					{
						free(colorbuffer);												// Delete it
					}

					if(imgrec->data != NULL)										// If there is Image data
					{
						free(imgrec->data);										// delete it
					}

					return GL_FALSE;														// Return failed
				}
			}
		}
	} while(currentpixel < pixelcount);													// Loop while there are still pixels left
	fclose(fTGA);																		// Close the file
	return GL_TRUE;																		// return success
}



//
// NAME: LoadRawFile
//
// ZWECK: Einlesen von rohen Bin�rdaten. Wird f�r Height-Maps ben�tigt
//
// KOMMENTAR:  
//
// R�CKGABEWERTE:
//			   -2 : Datei konnte nicht geladen werden
//			   -1 : Aus Datei konnte nicht gelesen werden
//				0 : kein Fehler
//
int LoadRAWFile(LPCSTR strName,		// Dateiname
				GLuint nSize,		// Gr��e (width * height)
				BYTE *pHeightMap)	// Bitmap-Array der gr��e nSize!
{
	FILE *pFile = NULL;

	// Datei im bin�ren Modus �ffnen
	pFile = fopen( strName, "rb" );
	if ( pFile == NULL )	
		return -2;

	// RAW File in einem Durchgang einlesen
	if ( fread( pHeightMap, 1, nSize, pFile ) == 0)
	{
		fclose(pFile);	
		return -2;
	}
	// After we read the data, it's a good idea to check if everything read fine.
	if( ferror( pFile ) )
		return -1;

	fclose(pFile);
	return 0;
}

