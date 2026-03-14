//////////////////////////////////////////////////////////////////////////
// devconsole.h

#ifndef _DEVCONSOLE_H
#define _DEVCONSOLE_H

// Filedeskriptor auf die Konsole holen, stdout umbiegen auf das Konsolenhandle
int getconsole();
// Ausgabe auf der Konsole
void devconout(const char *msg, ...);

#endif /* _DEVCONSOLE_H */