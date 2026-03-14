//////////////////////////////////////////////////////////////////////////
// Definiert Konstanten für die Display-Listen

#ifndef _DISPLAYLIST_H
#define _DISPLAYLIST_H

//////////////////////////////////////////////////////////////////////////
// Über Displaylisten:
//
// This is an unsigned int (since we can't have a negative base pointer) that will hold
// the ID for our display list.  It isn't so much of an ID, but it's easier to think of it
// that way.  We we create a display list, we start at 1.  If we create another one we
// then move the list pointer to 2.
//
// HINWEIS: Das Generieren der Listen sollte einmalig beim Initialisieren der Szene geschehen
//////////////////////////////////////////////////////////////////////////


// Gibt an, wie viele Listen generiert werden
#define OBJECT_LIST_COUNT		5

// Listen-IDs zum Zugriff über ein Array! NICHT ALS BASEPOINTER VERWENDEN!!
// müssen fortlaufend definiert werden, da dies den internen Listen-Zeiger (base-pointer
// betrifft, der einen gültigen Listeneintrag erwartet!
#define HEIGHTMAP_LIST_IDX		0
#define CUBE_LIST_IDX			1
#define VERDANAFONT_LIST_IDX	2
#define ARIALFONT_LIST_IDX		3
#define TIMESFONT_LIST_IDX		4

#endif /* _DISPLAYLIST_H */