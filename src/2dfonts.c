//////////////////////////////////////////////////////////////////////////
// 2Dfonts.cpp

#include "stdafx.h"
#include "2Dfonts.h"

//
//	NAME: CreateGLFontList
//
// ZWECK: Erstellt MAX_CHAR viele Display-List Einträge für die angegebene Schriftart
//
// KOMMENTAR: Gibt eine Display-ListenID zurück, die später für die Anzeige der Schrift
//			  benötigt wird.
//
// Gibt den Base-Pointer für die erstellte Display-Liste zurück
//
GLuint CreateGLFontList(HDC		hDC,
						LPCSTR	strFontName, 
						UINT	nHeight)	// Build Our Bitmap Font
{
	GLuint	fontListID = 0;						// This will hold the base ID for our display list
	HFONT	hFont, oldFont;						// This will store the handle to our font
	BOOL	bRet;								// Rückgabewert für die Fehlerbehandlung

	assert(hDC != NULL && "CreateGLFontList: invalid harware device context.");
	
	devconout("Creating font %s", strFontName);

	// Here we generate the list for each character we want to use.
	// This function then returns the base pointer, which will be 1 because
	// we haven't created any other lists.  If we generated another list after
	// this, the base pointer would be at 257 since the last one used was 256 (which is MAX_CHARS)
	fontListID = glGenLists(MAX_CHAR);					// Generate the list for the font
	devconout(", list id = %i", fontListID);
	assert( glIsList(fontListID) == GL_TRUE && "CreateGLFontList: invalid display list.");
	// Now we actually need to create the font.  We use a windows function called:
	// CreateFont() that returns a handle to a font (HFONT).  Our CreateOpenGLFont()
	// function allows us to pass in a name and height.  For simplistic reasons, I left
	// other options out, but feel free to add them to your function (like bold, italic, width..)

	hFont = CreateFont(	nHeight,							// Our desired HEIGHT of the font
		0,								// The WIDTH (If we leave this zero it will pick the best width depending on the height)
		0,								// The angle of escapement
		0,								// The angle of orientation
		FW_BOLD,						// The font's weight (We want it bold)
		FALSE,							// Italic - We don't want italic
		FALSE,							// Underline - We don't want it underlined
		FALSE,							// Strikeout - We don't want it strikethrough
		ANSI_CHARSET,					// This is the type of character set
		OUT_TT_PRECIS,					// The Output Precision
		CLIP_DEFAULT_PRECIS,			// The Clipping Precision
		ANTIALIASED_QUALITY,			// The quality of the font - We want anitaliased fonts
		FF_DONTCARE|DEFAULT_PITCH,		// The family and pitch of the font.  We don't care.
		strFontName);					// The font name (Like "Arial", "Courier", etc...)
	
	assert(hFont != NULL);

	// Now that we have created a new font, we need to select that font into our global HDC.
	// We store the old font so we can select it back in when we are done to avoid memory leaks.
	oldFont = (HFONT) SelectObject(hDC, hFont);
	
	assert(oldFont != NULL);

	
	// This function does the magic.  It takes the current font selected in
	// the hdc and makes bitmaps out of each character.  These are called glyphs.
	// The first parameter is the HDC that holds the font to be used.
	// The second parameter is the ASCII value to start from, which is zero in our case.
	// The third parameter is the ASCII value to end on (255 is the last of the ASCII values so we
	// subtract 1 from MAX_CHARS)
	// The last parameter is the base pointer for the display lists being used.

	bRet = wglUseFontBitmaps(hDC, 0, MAX_CHAR - 1, fontListID);		// Builds 255 bitmap characters
	devconout(", wglUseFontBitmaps: bRet %i, GetLastError: %i \n", bRet, GetLastError());
	
	// Tritt hier eine Assertion auf, dann wird die Schriftart nicht unterstützt
	// TODO: Die erste Schrift wird aus irgendwelchen Gründen nicht von wgl... erzeugt...
	//assert(bRet == TRUE);
	
	// Displayliste auf Existenz prüfen
	//assert( glIsList( fontListID ) == GL_TRUE );
	devconout("testing list ... glIsList ret = %i\n", glIsList( fontListID ));

	GL_ErrorReport();
	
	return fontListID;									// Return the ID to the display list to use later
}

//
//	NAME : glDrawText2D
//
//	ZWECK: 2 Dimensionalen Text an den angegebenen Viewport Koordinaten ausgeben
//
// KOMMENTAR:
//
void glDrawText2D(GLuint fontListID,		// Angabe des base-pointers um die Liste zu adressieren
				  int viewport_height,		// Höhe des Viewports, zur Positionierung benötigt
				  int x,					// Viewport x
				  int y,					// Viewport y
				  const char *strString,	// Auszugebender String
				  ...)						// optionale Formatspezifizierer
{
	char		strText[MAX_CHAR];						// This will hold our text to display
	va_list		argumentPtr;							// This will hold the pointer to the argument list

	// Auf Existenz der Displayliste prüfen
	assert( glIsList( fontListID ) == GL_TRUE && "glDrawText2D : Non existant display-list!");

	// If you have never used a va_list, listen up.  Remember printf()?
	// or sprintf()?  Well, you can add unlimited arguments into the text like:
	// printf("My name is %s and I am %d years old!", strName, age);
	// Well, that is what va_list's do.  

	// First we need to check if there was even a string given
	if (strString == NULL)								// Check if a string was given
		return;											// Don't render anything then

	// First we need to parse the string for arguments given
	// To do this we pass in a va_list variable that is a pointer to the list of arguments.
	// Then we pass in the string that holds all of those arguments.
	va_start(argumentPtr, strString);					// Parse the arguments out of the string

	// Then we use a special version of sprintf() that takes a pointer to the argument list.
	// This then does the normal sprintf() functionality.
	// GEÄNDERT: Sicherer mit StringCbPrintf
	//StringCbPrintf(strText, sizeof(strText)-1, strString, argumentPtr);	// Now add the arguments into the full string
	snprintf(strText, sizeof(strText)-1, strString, argumentPtr);
	va_end(argumentPtr);								// This resets and frees the pointer to the argument list.

	// Before we draw the text, we need to position it with our own function.
	PositionText2D(x, y, viewport_height);			// Call our own function to position the text on screen

	// Now, before we set the list base, we need to save off the current one.
	glPushAttrib(GL_LIST_BIT);							// This saves the list base information

	// Then we want to set the list base to the font's list base, which should be 1 in our case.
	// That way when we call our display list it will start from the font's lists'.
	glListBase(fontListID);							// This sets the lists base

	// Now comes the actually rendering.  We pass in the length of the string,
	// then the data types (which are characters so its a UINT), then the actually char array.
	// This will then take the ASCII value of each character and associate it with a bitmap.
	glCallLists( (GLsizei) strlen(strText) , GL_UNSIGNED_BYTE, strText);

	glPopAttrib();										// Return the display list back to it's previous state

	GL_ErrorReport();
}

//
//	NAME: PositionText2D
//
// ZWECK: Positioniert den 2 Dimensionalen Text fix (unabhängig von Translationen) im Raum
//
// KOMMENTAR:
// If you are to use this font code for your applications,
// you must be aware that you cannot position the font in 3D,
// which means you can't rotate and scale it.  That will be covered in
// the next font tutorial.  BUT, though that might be a drag, this code
// is useful because when you display the text, it will always be on top
// of everything else.  This is good if the camera is moving around, and you
// don't want the text to move.  If the text was positioned in 3D you would have
// to come up with a tricky way of making it always render in front of the camera.
// To do this, we need to set the Raster Position.  That is the position that OpenGL
// starts drawing at.  Since it's in floating point, it's not very intuitive, so what
// we do is create a new view port, and then always draw the text at (0, 0, 0) in that
// view port.  The weird part is that the Y is flipped, so (0, 0) is the bottom left corner.
// Below we do some simple math to flip it back to normal.
//
void PositionText2D( int x, int y, int viewport_height )
{

	// Before we create a new view port, we need to save the current one we have.
	// This saves our transform (matrix) information and our current viewport information.
	// At the end of this function we POP it back.
	glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );

	// Here we use a new projection and modelview matrix to work with.
	glMatrixMode( GL_PROJECTION );						// Set our matrix to our projection matrix
	glPushMatrix();										// Push on a new matrix to work with
	glLoadIdentity();									// reset the matrix
	glMatrixMode( GL_MODELVIEW );						// Set our matrix to our model view matrix
	glPushMatrix();										// Push on a new matrix to work with
	glLoadIdentity();									// Reset that matrix

	// Because the Y is flipped, we want 0 to be at the top, not bottom.
	// If we subtract the font height from the screen height, that should display the
	// font at the top of the screen (if they passed in 0 for Y), but then we subtract
	// the Y from that to get the desired position.  Since the font's drawing point is
	// at the base line of the font, we needed to subtract the font height to make sure
	// if they passed in (0, 0) it wouldn't be off screen.  If you view this in window mode,
	// the top of the window will cut off part of the font, but in full screen it works fine.
	// You just need to add about 25 to the Y to fix that for window mode.

	// TODO: Generalisieren...
	//y = SCREEN_HEIGHT - FONT_HEIGHT - y;				// Calculate the weird screen position
	
	// SCREEN_HEIGHT ist falsch. Es muss die viewport-Höhe sein!
	y = viewport_height - FONT_HEIGHT - y;

	// Now we create another view port (that is why we saved the old one above).
	// Since glViewPort takes the lower LEFT corner, we needed to change the Y
	// to make it more intuitive when using PositionText().  We minus 1 from the X and Y
	// because 0 is taken into account with the position.  The next 2 parameters are set
	// to 0 for the width and height so it will always draw in the middle of that position.
	// glRasterPos4f() takes (0, 0, 0) as the middle of the viewport, so if we give it a small
	// width/height it will draw at the X and Y given.  Sounds strange, to test this, try
	// using glRasterPos4f(0, 0, 0, 1) instead of PositionText() and you will see, everything
	// will be drawn from the middle.

	glViewport( x - 1, y - 1, 0, 0 );					// Create a new viewport to draw into

	// This is the most important function in here.  This actually positions the text.
	// The parameters are (x, y, z, w).  w should always be 1 , it's a clip coordinate.
	// don't worry about that though.  Because we set the projection and modelview matrix
	// back to the beginning (through LoadIdentity()), the view port is looking at (0, 0, 0).
	// This is the middle, so if we set the drawing position to the middle, it will draw at our
	// X and Y because the width/height of the viewport is 0, starting at X and Y.
	// You can actually call this function (or glRasterPos2f(0, 0)) instead of PositionText(),
	// but it is in floating point and doesn't work as nicely.  You will see why if you try.

	glRasterPos4f( 0, 0, 0, 1 );						// Set the drawing position

	// Now that we positioned the raster position, any text we draw afterwards will start
	// from that position.  Now we just have to put everything else back to normal.

	glPopMatrix();										// Pop the current modelview matrix off the stack
	glMatrixMode( GL_PROJECTION );						// Go back into projection mode
	glPopMatrix();										// Pop the projection matrix off the stack

	glPopAttrib();										// This restores our TRANSFORM and VIEWPORT attributes
	
	GL_ErrorReport();
}
