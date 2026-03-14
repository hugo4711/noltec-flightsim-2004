// main.cpp : Definiert den Einstiegspunkt f�r die Anwendung.
//

#include "stdafx.h"
#include <strsafe.h>
#include "main.h"
#include "GLscene.h"		// OpenGL Szenen-Rendering
#include "camera.h"			// Kamerasteuerung f�r die GL-Szene

#define MAX_LOADSTRING 100



extern float frame_interval;					// in GLScene.c definiert. Zeit in ms die 
												// das Rendern eines Frames dauert

// Globale Variablen:
HDC g_hDC;										// Device-Context
HGLRC g_hGLRC;									// GL Render-Context
HINSTANCE hInst;								// Aktuelle Instanz
TCHAR szTitle[MAX_LOADSTRING];					// Titelleistentext
TCHAR szWindowClass[MAX_LOADSTRING];			// Klassenname des Hauptfensters

// Status-Variablen
BOOL move_by_mouse	 = FALSE;   // gibt an, ob die Maussteuerung aktiviert ist (siehe WM_MOUSEMOVE)

// Vorw�rtsdeklarationen der in diesem Codemodul enthaltenen Funktionen:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
BOOL				SetDCPixelFormat(HDC hDC); // Pixelformat f�r den Device-Context einstellen
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				FullScreen(HWND hWnd); 
void				PollKeyState();
int					GetConsoleHandle();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	//BOOL bRet;
	MSG msg;
	HACCEL hAccelTable;

	// Globale Zeichenfolgen initialisieren
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PHYSICSENGINEGL, szWindowClass, MAX_LOADSTRING);
	
#ifdef _DEBUG
	//////////////////////////////////////////////////////////////////////////
	// Developer-Konsole einrichten
	//////////////////////////////////////////////////////////////////////////
	if(getconsole() == NULL)
		MessageBox(NULL, "console failed.", "Critical", MB_OK | MB_ICONEXCLAMATION);
	else
		devconout("DEVCONSOLE: initialized.\nloading app...\n");
#endif
	
	// Fensterklasse registrieren
	if(!MyRegisterClass(hInstance))
	{
		MessageBox(NULL, "Instance could not be registered.", "Critical", MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}

	// Anwendungsinitialisierung ausf�hren:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		MessageBox(NULL, "Initialization failed.", "Critical", MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_PHYSICSENGINEGL);

	/*
	// Hauptmeldungsschleife (blocking):
	// Solange, bis WM_QUIT gesendet wird. Dann liefert GetMessage 0
	while ( ( bRet = GetMessage(&msg, NULL, 0, 0) ) != 0 ) 
	{
		if(bRet == -1) {
			// Fehler behandeln und evtl. Programm beenden
			// siehe GetMessage in MSDN
			MessageBox(NULL, "Unknown Message State.", "Critical", MB_OK | MB_ICONEXCLAMATION);
			return -1;
		}
		if ( !TranslateAccelerator(msg.hwnd, hAccelTable, &msg) ) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			// Keyboard polling durchf�hren
			PollKeyState();
		}
	}*/
	
	devconout("window initialized. going to message loop...\n");

	// Hauptmeldungsschleife, die durch PeekMessage erm�glicht,
	// noch andere Dinge zu tun, als nur auf Windows-Nachrichten
	// zu warten (non-blocking = busy waiting
	while(TRUE)											
	{													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{ 
			if(msg.message == WM_QUIT)					
				break;
			if ( !TranslateAccelerator(msg.hwnd, hAccelTable, &msg) ) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}					
		}
		else											
		{ 
			
			// OpenGL Zeichenfunktion aufrufen
			DrawGLScene();
			// Buffer austauschen, da Double-Buffering; nun wird tats�chlich auf DC gezeichnet
			SwapBuffers(g_hDC);
			// Tastaturpuffer pollen
			PollKeyState();
		} 
	}

	// Hierher gehts nur mit einer WM_QUIT Message
	return (int) msg.wParam;
}



//
//  FUNKTION: MyRegisterClass()
//
//  ZWECK: Registriert die Fensterklasse.
//
//  KOMMENTAR:
//
//    Sie m�ssen die Funktion zu verwenden, wenn Sie m�chten, dass der Code
//    mit Win32-Systemen kompatibel ist, bevor die Funktion 'RegisterClassEx'
//    zu Windows 95 hinzugef�gt wurde. Der Aufrud der Funktion ist wichtig,
//    damit die kleinen Symbole, die mit der Anwendung verkn�pft sind,
//    richtig formatiert werden.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_PHYSICSENGINEGL);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_PHYSICSENGINEGL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex); 
}

//
//   FUNKTION: InitInstance(HANDLE, int)
//
//   ZWECK: Speichert das Instanzenhandle und erstellt das Hauptfenster.
//
//   KOMMENTAR:
//
//        In dieser Funktion wird das Instanzenhandle in einer globalen Variablen gespeichert, und das
//        Hauptprogrammfenster wird erstellt und angezeigt.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Instanzenhandle in der globalen Variablen speichern

	
	/*
	// Fullscreen-Mode oder nicht?

	if( MessageBox(NULL, "Soll der Fullscreen-Modus verwendet werden?",
					"?", MB_YESNO | MB_ICONQUESTION) == IDYES )
	{
		// Globales Flag setzen
		bFullscreen = TRUE;

		DEVMODE deviceMode;
		// Aktuelle Bildschirmaufl�sung ermitteln
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &deviceMode);
		
		// Neue Einstellungen anwenden
		if (ChangeDisplaySettings(&deviceMode,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			return FALSE;
		}
	}*/

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	


	return TRUE;
}

// Funktion: SetDCPixelFormat
//
// ZWECK: Anpassen des Pixelformats f�r das Rendern in der OpenGL Scene
//
// KOMMENTAR: TODO: Farbtiefe und Tiefenbuffer sollten per Config-File eingestellt werden.
//			  Gibt FALSE zur�ck, wenn irgendetwas beim Ausw�hlen oder Einstellen
//			  des Pixelformatdeskriptors fehlschl�gt
//			  Das PixelFormat darf nur EINMAL aufgerufen werden (siehe MSDN SetPixelFormat)
BOOL SetDCPixelFormat(HDC hDC) {
	int nPixelFormat;
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
			1,								// Version of this structure
			PFD_DRAW_TO_WINDOW |			// Draw to window (not bitmap)
			PFD_SUPPORT_OPENGL |			// Support OpenGL calls
			PFD_DOUBLEBUFFER,				// Double -buffered mode
			PFD_TYPE_RGBA,					// RGBA Color mode
			32,								// Want 32bit color
			0,0,0,0,0,0,					// Not used to select mode
			0,0,							// Not used to select mode
			0,0,0,0,0,						// Not used to select mode
			16,								// Size of depth buffer
			0,								// Not used to select mode
			0,								// Not used to select mode
			PFD_MAIN_PLANE,					// Draw in main plane
			0,								// Not used to select mode
			0,0,0							// Not used to select mode
	}; 

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);
	if( !nPixelFormat )
		return FALSE;
	// Set the pixel format for the device context
	return SetPixelFormat(hDC, nPixelFormat, &pfd);
}

//
//  FUNKTION: WndProc(HWND, unsigniert, WORD, LONG)
//
//  ZWECK:  Verarbeitet Meldungen vom Hauptfenster.
//
//  WM_COMMAND	- Verarbeiten des Anwendungsmen�s
//  WM_PAINT	- Zeichnen des Hauptfensters
//  WM_DESTROY	- Beenden-Meldung anzeigen und zur�ckgeben
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	//static HDC hDC;						// Device-Context zum Zeichnen, permanent
	//static HGLRC hGLRC;					// GL Render-Context, permanent
	RECT clientRect;					// Client-Rect ist der Zeichenbereich f�r die OpenGL-Scene

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Men�auswahl bearbeiten:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CREATE:
		devconout("WM_CREATE : creating window\n");

		// Initialisierungsarbeiten beim Erzeugen des Fensters
		// Ger�tekontext sichern
		g_hDC = GetDC(hWnd);
		// Pixelformat anpassen
		SetDCPixelFormat(g_hDC);
		// Renderkontext erzeugen und ihn zum Aktuellen machen
		g_hGLRC = wglCreateContext(g_hDC);
		wglMakeCurrent(g_hDC, g_hGLRC);
		// OpenGL Renderscene initialisieren
		GetClientRect(hWnd, &clientRect);
		// Initialisierung der OpenGL-Scene (Texturen laden usw.)
		InitGL(g_hDC, clientRect.right, clientRect.bottom);
		// Einen Timer installieren, der jede 1ms eine WM_TIMER Message sendet,
		// die ben�tigt wird, um die Scene neu zu rendern. Das geschieht durch
		// Kennzeichnen des Fensterbereichs mit InvalidateRect() bei WM_TIMER und dadurch
		// ausgel�stes Senden von WM_PAINT, dadurch werden Animationen m�glich
		/*
		SetTimer(hWnd,		// Fenster in dessen Kontext der Timer existiert
				 1,			// TimerID
				 1,			// Zeit, die angibt wie h�ufiger der Timer aufgerufen wird (in ms)
				 NULL);		// Callback-Funktion, wenn NULL, dann werden WM_TIMER Messages generiert
		*/
		break;
	/*case WM_PAINT:
		// Validieren der Zeichenfl�che. F�hrt dazu, das wieder eine WM_PAINT Message gesendet wird
		// So wird ein kontinuierlicher Strom an WM_PAINTs erzeugt, bis die Zeichnfl�che als valide
		// erkannt wurde (durch GDI). Wird nur bei "ineffizienter" Renderingmethode ben�tigt, oder wenn
		// noch per GDI im DC gezeichnet werden soll.
		//ValidateRect(hWnd,NULL);
		break;
	*/
	case WM_SIZE:
		// OpenGL-Renderscene neu berechnen und an die neue Fenstergr��e anpassen.
		ReSizeGLScene( LOWORD(lParam), HIWORD(lParam) );
		break;
	case WM_KEYDOWN:
		//////////////////////////////////////////////////////////////////////////
		// Hier keine zeitkritischen Tasten abfragen! Geschieht in PollKeyState!!
		//////////////////////////////////////////////////////////////////////////
		
		// wParam entspricht dem Scan-Code f�r virtuelle Key-Codes (keine normalen Tasten)
		// VK_XXX bezeichnet virtuelle Keycodes, die bei sondertasten generiert werden
		// und auf den ASCII-Code gemappt werden.
		switch(wParam) {
		case VK_ESCAPE:
			// ESC beendet das Programm sauber per WM_DESTROY
			PostMessage(hWnd, WM_DESTROY, NULL, NULL);
			break;
		}
		break;
	case WM_CHAR:
		//////////////////////////////////////////////////////////////////////////
		// Hier keine zeitkritischen Tasten abfragen! Geschieht in PollKeyState!!
		//////////////////////////////////////////////////////////////////////////

		// WM_CHAR liegt dann in der Message-Queue vor, wenn eine "Character-Taste" gedr�ckt wurde
		switch(wParam) {
		case 'F':
		case 'f':
			// Vollbilddarstellung
			FullScreen(hWnd);
			break;
		case 'M':
		case 'm':
			// Mauscursor ausblenden, ansonsten unsch�nes hin- und herspringen des
			// Cursors bei der Mausbewegung
			if(!move_by_mouse)
				ShowCursor(FALSE);
			else
				ShowCursor(TRUE);
			// Flag setzen
			move_by_mouse = !move_by_mouse;
			break;
		case 'R':
		case 'r':
			// Kamera wieder in die Ursprungsposition fahren
			ResetSimulation();
			break;
		case 'P':
		case 'p':
			PauseSimulation();
			break;
		case 'V':
		case 'v':
			// Vsync umschalten
			ToggleVSync();
			break;
		}

		break;
	case WM_LBUTTONDOWN:
		// linker Maustasten-Klick
		ToggleWireFrameView();
		break;
	case WM_MOUSEMOVE:
		// Wenn das Flag gesetzt wurde (Taste M)
		// dann wird die Maussteuerung aktiviert
		if(move_by_mouse)		
			SetCameraViewByMouse();
		break;
	case WM_KEYUP:
		break;
	case WM_TIMER:
		// Der Timer wird, wie in SetTimer angegeben alle 1ms aufgerufen 
		// und generiert konstante WM_TIMER-Messages
		// Einzige Funktion die der Timer hat, ist, daf�r zu sorgen, dass die Scene
		// permanent neu gerendert wird. Daf�r werden WM_PAINT-Messages generiert
		
		// Ineffiziente Methode zum Rendern...

		// PostMessage(hWnd, WM_PAINT, NULL, NULL);
		
		break;
	case WM_DESTROY:
		devconout("cleaning up...\n");
		// Aufr�umarbeiten:
		// Timer entfernen, den wir erzeugt hatten
		//KillTimer(hWnd,101);
		// Aktuellen Render-Kontext deselektieren und l�schen
		wglMakeCurrent(g_hDC,NULL);
		wglDeleteContext(g_hGLRC);
		// Aufr�umen der Scene-Spezifischen Daten
		KillGLScene();
		// Applikation kann nun beendet werden
		PostQuitMessage(0); // Sendet WM_QUIT
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Meldungshandler f�r Infofeld.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPCSTR status_text = NULL;

	switch (message)
	{
	case WM_INITDIALOG:
		status_text = (LPCSTR) malloc(1024);
		
		//////////////////////////////////////////////////////////////////////////
		// Ausgeben von Statusinformationen bzgl. OpenGL
		//////////////////////////////////////////////////////////////////////////
		
		// Safe String Funktionen benutzen um den String zusammenzubauen
		StringCbCopyN((char *) status_text,
					  1023,
					  (LPCSTR) glGetString(GL_VENDOR),
					  1023 );
		StringCbCat((char *) status_text, 1023, ", Version: ");
		StringCbCat((char *) status_text, 1023,  (LPCSTR) glGetString(GL_VERSION) );
		StringCbCat((char *) status_text, 1023, "\nRenderer: ");
		StringCbCat((char *) status_text, 1023,  (LPCSTR) glGetString(GL_RENDERER) );
		StringCbCat((char *) status_text, 1023, "\nExtensions:\n");
		StringCbCat((char *) status_text, 1023,  (LPCSTR) glGetString(GL_EXTENSIONS) );
		
		// Statusinformationen f�r die OpenGL-Infos im Static anzeigen
		SetDlgItemText(hDlg, IDC_STATUS, status_text);
		
		// status_text wird nicht l�nger ben�tigt
		free((void *)status_text);
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//
//	NAME : Fullscreen
//
//	ZWECK: Erm�glicht Vollbilddarstellung zur Laufzeit
//
void FullScreen(HWND hWnd) {
	static BOOL		render_fullscreen = FALSE;  // gibt an, ob Vollbilddarstellung eingeschaltet ist, oder nicht
	static LONG		oldStyle;					// Wenn in die Vollbilddarstellung gewechselt wird, alten
												// Window-Style sichern
	static RECT		oldWndRect;					// Alte Gr��e des Fensters sichern
	static HMENU    oldMenu;					// Men�leiste sichern
	BOOL			bRet;						// R�ckgabewert zur Fehlerpr�fung
	
	// Pr�fen ob im Vollbild gerendert wird
	if(!render_fullscreen)
	{
		//////////////////////////////////////////////////////////////////////////
		// In Vollbild wechseln
		//////////////////////////////////////////////////////////////////////////
		
		// vorherige Gr��e des Fensters sichern
		bRet = GetWindowRect(hWnd, &oldWndRect);
		assert(bRet == TRUE);

		// Fenster in den Vordergrund holen
		bRet = SetForegroundWindow(hWnd);
		assert(bRet == TRUE);

		// Men�leiste entfernen, vorher das original sichern
		oldMenu = GetMenu(hWnd);
		assert(oldMenu != NULL);

		// entfernen
		bRet = SetMenu(hWnd, NULL);
		assert(bRet == TRUE);

		// MSDN: If you have changed certain window data using SetWindowLong,
		// you must call SetWindowPos to have the changes take effect.
		// Use the following combination for uFlags:
		// SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED.
		// 
		// ACHTUNG : Mit dieser MSDN-Information funktioniert es ncht!
		oldStyle = SetWindowLong(hWnd,
								 GWL_STYLE,
								 WS_POPUP);
		assert(oldStyle != NULL);

		// MSDN: Width and height of the screen of the primary display monitor, in pixels.
		// These are the same values obtained by calling GetDeviceCaps(hdcPrimaryMonitor,
		// HORZRES/VERTRES).
		// HWND_TOPMOST = immer im Vordergrund
		bRet = SetWindowPos(hWnd,HWND_TOP,0,0,
							GetSystemMetrics(SM_CXSCREEN),
							GetSystemMetrics(SM_CYSCREEN),
							SWP_SHOWWINDOW | SWP_FRAMECHANGED);
		assert(bRet == TRUE);

		// Mauscursor verbergen
		ShowCursor(FALSE);

		render_fullscreen = !render_fullscreen;
	} 
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// Zur�ck in den Fenster-Modus
		//////////////////////////////////////////////////////////////////////////
		
		// Men�leiste wiederherstellen
		bRet = SetMenu(hWnd, oldMenu);
		assert(bRet == TRUE);

		SetWindowLong(hWnd,
			GWL_STYLE,
			oldStyle
			);

		bRet = SetWindowPos(hWnd,HWND_TOP,
							oldWndRect.left,					// x-Position
							oldWndRect.top,						// y-Position
							oldWndRect.right - oldWndRect.left,	// width
							oldWndRect.bottom - oldWndRect.top,	// height
							SWP_SHOWWINDOW | SWP_FRAMECHANGED);
		assert(bRet == TRUE);

		// Mauszeiger wieder anzeigen
		ShowCursor(TRUE);

		render_fullscreen = !render_fullscreen;
	}
}



//
//	NAME : PollKeyState
//
//	ZWECK: Hardware-Tastaturpuffer pollen und Tastenstatus abfragen.
//
//  KOMMENTAR : Das funktioniert effizienter, weil auch parallel gedr�ckte
//				Tasten verarbeitet werden k�nnen. Wenn der Benutzer eine
//				Bewegung vollf�hren m�chte und daf�r eine Taste l�nger
//				dr�ckt, dann werden zuviele WM_KEYDOWN Messages generiert,
//				als das eine zweite parallel gedr�ckte Taste erkannt werden k�nnte.
//				Wird dann eine zweite Taste parallel gedr�ckt, dann wird der WM_KEYDOWN
//				Strom der Ersten unterbrochen; damit auch die Bewegung des Benutzers...
//				Au�erdem werden f�r Buchstabentasten gesondert WM_CHAR Messages generiert,
//				die extra abgefragt werden m�ssen. Das erzeugt zuviel Overhead.
//
//				Hier sollten zeitkritische, nicht Anwendungsrelevante Tasten abgefragt werden
//				Die Problematik ist nur bei Tastatureingaben relevant. Mausaktionen sollten
//				und k�nnen problemlos in der WinProc gehandelt werden
void PollKeyState()
{	
	// MSDN �ber GetAsyncKeyState :
	// If the function succeeds, the return value specifies whether the key was pressed
	// since the last call to GetAsyncKeyState, and whether the key is currently up or down.
	// If the most significant bit is set, the key is down, and if the least significant bit
	// is set, the key was pressed after the previous call to GetAsyncKeyState.
	// -> Kann ALLE virtuellen Keycodes pr�fen. Auch Mausklicks

	// durch den das Bitweise AND wird gepr�ft, ob das HIGH-ORDER Bit gesetzt ist. Falls ja,
	// ist die Taste gedr�ckt worden. 0x8000 = 10000000 (bin)

	// Geschwindigkeit in Abh�ngigkeit des Frame-Intervals bestimmen
	float speed = MOVE_SPEED /** frame_interval*/;

	// Steuerung des Flugzeugs in Ausgangsposition
	ZeroRudder();
	ZeroAilerons();
	ZeroElevators();

	// TODO: Implementation eines Mechanismus der es erlaubt zwischen Free-Look und Flugzeugsteuerung
	//		 hin und her zu schalten.

	if( (GetAsyncKeyState(VK_UP) & 0x8000) || ( GetAsyncKeyState('W') & 0x8000 ) )
	{
		// Nase runter
		PitchDown();
	}
	if( (GetAsyncKeyState(VK_DOWN) & 0x8000) || ( GetAsyncKeyState('S') & 0x8000 ) )
	{
		// Nase hoch
		PitchUp();
	}
	if( (GetAsyncKeyState(VK_LEFT) & 0x8000) || ( GetAsyncKeyState('A') & 0x8000 ) )
	{	
		// Um die Y-Achse, in positiver Richtung LINKSherum rotieren
		//RotateCameraQ(speed, 0, 1, 0);
		RollLeft();
	}
	if( (GetAsyncKeyState(VK_RIGHT) & 0x8000) || ( GetAsyncKeyState('D') & 0x8000 ) )
	{
		// Um die Y-Achse in neg. Richtung RECHTSherum rotieren
		//RotateCameraQ(-speed, 0, 1, 0);
		RollRight();
	}
	if( (GetAsyncKeyState(VK_SPACE) & 0x8000) )
	{	
		// Rakete feuern
		LaunchMissile();
	}
	if( (GetAsyncKeyState(VK_PRIOR) & 0x8000) ) //BILD_HOCH
	{
		// Schub erh�hen
		IncThrust();
	}
	if( (GetAsyncKeyState(VK_NEXT) & 0x8000) ) //BILD_RUNTER
	{
		// Schub erh�hen
		DecThrust();
	}
}

