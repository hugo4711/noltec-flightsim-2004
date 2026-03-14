//////////////////////////////////////////////////////////////////////////
// Ausgaben auf der Developer-Konsole

#include "stdafx.h"
#include <strsafe.h>
#include <io.h>
#include <fcntl.h>

#ifdef _BETA_STATUS
// Logfile schreiben
FILE* outfile;
#endif /*_BETA_STATUS */

//
//	NAME : getconsole
//
//	ZWECK: Console allokieren und ein Handle darauf ermitteln.
//
// KOMMENTAR: Zu Developmentzwecken erforderliche Statusmeldungen werden zus�tzlich
//			  zur GUI in einem Konsolenfenster ausgegeben
//			 Siehe KNOWLEDGE-BASE: Calling CRT Output Routines from a GUI Application
//
//			 Gibt einen Filedeskriptor zur�ck, so dass mit normalen stdio Funktionen
//			 Ausgaben m�glich sind. NULL im Fehlerfall
//
//			 Ben�tigt io.h, fcntl.h
int getconsole() 
{
	int hCrt;
	int ret;
	FILE *hf;

#ifdef _BETA_STATUS
	// Logfile schreiben
	outfile = fopen("betaversion.log","w");
#endif /*_BETA_STATUS */

	// The AllocConsole function allocates a new console for the calling process
	if(!AllocConsole())
		return NULL;

	// Handle auf eine Output-Konsole in einen C-Filedeskriptor umwandeln zur Textausgabe

// Zeigerverk�rzungs-Warnung unterdr�cken. HANDLE -> long muss sein!
// Tritt nur bei striktem C-Compilieren auf
#pragma warning(disable: 4311) 
	
	hCrt = _open_osfhandle( (long) GetStdHandle(STD_OUTPUT_HANDLE),	_O_TEXT	);
	// File (console) zum Schreiben �ffnen
	hf = _fdopen( hCrt, "w" );
	// stdout auf die neu allokierte Konsole umleiten
	*stdout = *hf;
	// Ungepufferte Ausgabe auf die Konsole
	ret = setvbuf( stdout, NULL, _IONBF, 0 );

	return hCrt;
}

//
//
//	ZWECK: Statusmeldungen auf der Konsole ausgeben (nur wenn DEBUG-Version)
//
void devconout(const char *fmt,  ...)
{
	va_list		argptr;
	// Das ist nicht optimal. Buffer Overflow... (aber hier nur development Zwecke!)
	char		msg[4096];	

	va_start (argptr,fmt);
	StringCbVPrintfA(msg,sizeof(msg),fmt,argptr);
	va_end (argptr);

#ifdef _BETA_STATUS
	// Logfile schreiben
	fprintf(outfile, msg);
#endif /*_BETA_STATUS */

#ifndef _DEBUG
	return;			// don't confuse non-developers with techie stuff...
#endif

	printf(msg);

}

