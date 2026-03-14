//////////////////////////////////////////////////////////////////////////
// Ausgaben auf der Developer-Konsole
// Sonstige Profiling/Debugging - Makros

#include "../stdafx.h"
#include <strsafe.h>
#include <io.h>
#include <fcntl.h>

// Profiling-Variable
#ifdef _DEBUG
int		   profile_vertex_count = 0; // zum Z�hlen gerenderter Verticies
#endif

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
	outfile = fopen("output.log","w");
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

#ifndef _DEBUG
	return;			// don't confuse non-developers with techie stuff...
#endif

	va_start (argptr,fmt);
	StringCbVPrintfA(msg,sizeof(msg)-1,fmt,argptr);
	va_end (argptr);

#ifdef _BETA_STATUS
	// Logfile schreiben
	fprintf(outfile, msg);
#endif /*_BETA_STATUS */

	printf(msg);

}

//
// Ausgeben s�mtlicher aufgetretener OpenGL Fehler (aus NVIDIA Texture Compression Code)
// (nur wenn DEBUG Version kompiliert wird, ansonsten wird die Funktion durch nichts ersetzt)
//
#ifdef _DEBUG //_DEBUG

void GL_ErrorReport_Debug(const char* file, int line)
{
	GLuint errnum;
	const GLubyte *errstr;

	while (errnum = glGetError()) 
	{

#if 1
		// Daf�r sorgen, dass die Fehlermeldung nur einmal ausgegeben wird
		// Das ist in Schleifen von Vorteil
		static char filename[256];
		static int id = NULL;

		// Wenn der Dateiname ungleich dem zuvor gespeicherten ODER
		// Der Fehlercode ungleich ist, dann merken
		if(strncmp(file, filename, sizeof(filename)-1) != 0 || id != errnum)
		{
			strncpy(filename, file, sizeof(filename)-1);
			id = errnum;
		} else
		{
			// Fehlerstack l�schen
			while(errnum = glGetError());
			break;
		}

#endif

		// Ausgabe des Dateinamens und Zeilennummer des Logs.
		devconout("---\nERROR: %s, %d: ",file, line);
		// glut API nutzen um den Fehlercode in einen String umzuwandeln
		errstr = gluErrorString(errnum);
		devconout("%s:%d", (const char *)errstr, errnum);
		devconout("\n---\n");
	}

	return;

}
#else
#define GL_ErrorReport();
#endif /*_DEBUG*/


