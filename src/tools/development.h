//////////////////////////////////////////////////////////////////////////
// devconsole.h

#ifndef _DEVCONSOLE_H
#define _DEVCONSOLE_H


// Einfaches Profiling der Verticies über Makrodefinitionen
// die die Originalfunktionen ersetzen
#ifdef _DEBUG
extern int profile_vertex_count;
#define profile_glVertex3f(a,b,c); \
		profile_vertex_count++; glVertex3f(a,b,c);
#define profile_glVertex3i(a,b,c); \
		profile_vertex_count++; glVertex3i(a,b,c);
#define profile_glVertex2f(a,b); \
		profile_vertex_count++; glVertex2f(a,b);
#define profile_glVertex2d(a,b); \
		profile_vertex_count++; glVertex2d(a,b);
#define profile_addVertexCount(a); \
		profile_vertex_count+=a;
#define profile_getVertexCount() \
		profile_vertex_count
#define profile_resetVertexCount(); \
		profile_vertex_count=0;
#else
#define profile_glVertex3f(a,b,c); glVertex3f(a,b,c);
#define profile_glVertex3i(a,b,c); glVertex3i(a,b,c);
#define profile_glVertex2f(a,b); glVertex2f(a,b);
#define profile_glVertex2d(a,b); glVertex2d(a,b);
#define profile_addVertexCount(a);
#define profile_getVertexCount(); 
#define profile_resetVertexCount();
#endif /* _DEBUG */


// Filedeskriptor auf die Konsole holen, stdout umbiegen auf das Konsolenhandle
int getconsole();

// Ausgabe auf der Konsole
void devconout(const char *msg, ...);

#ifdef _DEBUG
// Ausgeben sämtlicher aufgetretener OpenGL Fehler
// Zum Ausgeben des Reports wird ausschließlich das Makro verwendet!
#define GL_ErrorReport(); GL_ErrorReport_Debug(__FILE__, __LINE__);
void GL_ErrorReport_Debug(const char* file, int errorid);
#else
#define GL_ErrorReport();
#endif


#endif /* _DEVCONSOLE_H */
