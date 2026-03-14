//////////////////////////////////////////////////////////////////////////
// GDI-Funktionalitäten
// Anmerkung: Veraltet

#include "stdafx.h"
#include "GDIfunctions.h"

//
//	NAME: gdiTextOut
//
//	ZWECK: Text auf dem Device-Context ausgeben
//
//	KOMMENTAR: Diese Funktion kann zum GDI-basierten Zeichnen auf einem Gerätekontext
//			   verwendet werden, auf dem bereits mit OpenGL gerendert wird. Die Spec erlaubt
//			   ein solches Vorgehen (s. OpenGL-Superbible S.110)
//
//	RÜCKGABE: FALSE - Wenn Erzeugen der Schrift oder des Pinsels fehlschlug
//
BOOL gdiTextOut(HDC hDC,				// Gerätekontext
				//HWND hWnd,				// Handle zum Fenster
				int x,					// x-Position
				int y,					// y-Position
				LPCTSTR lpString,		// Auszugebender String
				LPCTSTR fontname,		// Schriftart
				int nHeight,			// Größe der Schrift
				COLORREF foreground,	// Vordergrundfarbe (RGB-Makro nutzen)
				COLORREF background		// Hintergrundfarbe (RGB-Makro nutzen), NULL = Transparent
				)		
{
	HFONT hFont = NULL, hFontOld = NULL;			 // Alte und neue Schrift
	COLORREF oldTextColor = NULL, oldBkColor = NULL; // Alte Farben
	
	/*

	// MemoryDC für Double Buffering
	// Kompatiblen MemoryDC, basierend auf dem aktuellen Hardware DC erzeugen
	// Siehe "memory device context" in MSDN
	HDC memDC = CreateCompatibleDC(hDC);
	if(!memDC)
		return FALSE;

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	// Bitmap im Speicher erzeugen in dem dann gezeichnet wird
	HBITMAP memBMP = CreateCompatibleBitmap(hDC, clientRect.right, clientRect.bottom);
	if(!memBMP)
	{
		// Memory-DC wieder freigeben
		DeleteDC(memDC);	
		return FALSE;
	}

	// Bitmap in den Memory-Kontext selektieren
	// ab hier wird dann nur noch im Speicher gezeichnet
	SelectObject(memDC, memBMP);

	*/

	// Schrift erzeugen
	hFont = CreateFont(
						nHeight,			       // nHeight
						0,                         // nWidth
						0,                         // nEscapement
						0,                         // nOrientation
						FW_NORMAL,                 // nWeight
						FALSE,                     // bItalic
						FALSE,                     // bUnderline
						0,                         // cStrikeOut
						ANSI_CHARSET,              // nCharSet
						OUT_DEFAULT_PRECIS,        // nOutPrecision
						CLIP_DEFAULT_PRECIS,       // nClipPrecision
						DEFAULT_QUALITY,           // nQuality
						DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
						fontname);                 // lpszFacename)
	
	// Wenn Schrift nicht erzeugt werden konnte
	if(hFont == NULL)
		return FALSE;
	
	// Font  in den DC auswählen
	hFontOld = (HFONT) SelectObject(hDC, hFont);
	
	// Textfarbe anpassen
	oldTextColor = SetTextColor(hDC, foreground);
	
	// Transparent, wenn Hintergrundfarbe nicht beim Funktionsaufruf angegeben
	if(background == NULL) {
		// Hintergrundmodus auf Transparent stellen, damit der Hintergrund durchscheint
		if( SetBkMode(hDC, TRANSPARENT) == 0)
			return FALSE;
	} else {
		oldBkColor = SetBkColor(hDC, background);
	}

	// Ausgeben des lpStrings, mit der Länge des Strings
	TextOut(hDC, x, y, lpString, lstrlen(lpString));

	// Die Schrift und der Pinsel wird nicht länger gebraucht.
	// Wiederherstellen der alten Einstellungen
	DeleteObject(hFont);
	SelectObject(hDC, hFontOld);
	SetTextColor(hDC, oldTextColor);
	// Wenn Hintergrundfarbe eingestellt angegeben wurde, dann
	// Farbe zurücksetzen
	if(oldBkColor)
		SetTextColor(hDC, oldBkColor);

	// The BitBlt function performs a bit-block transfer of the color data
	// corresponding to a rectangle of pixels from the specified source device
	// context into a destination device context.
	// BitBlt(hDC,0,0,clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);

	//DeleteDC(memDC);
	//DeleteObject(memBMP);

	return TRUE;
}