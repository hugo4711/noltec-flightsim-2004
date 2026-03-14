// GDI Funktionalitäten, die auf den Device-Context angewendet werden können

#pragma once

// Textausgabe auf dem Device-Context
BOOL gdiTextOut(HDC hDC,				// Gerätekontext
				//HWND hWnd,				// Handle auf das Fenster in dem gezeichnet wird
				int x,					// x-Position
				int y,					// y-Position
				LPCTSTR lpString,		// Auszugebender String
				LPCTSTR fontname,		// Schriftart
				int nHeight,			// Größe der Schrift
				COLORREF foreground,	// Vordergrundfarbe (RGB-Makro nutzen)
				COLORREF background		// Hintergrundfarbe (RGB-Makro nutzen)
				);