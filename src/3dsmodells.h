//////////////////////////////////////////////////////////////////////////
// 3D Studio Max Modellkonstanten

#ifndef _3DSMODELLS_H
#define _3DSMODELLS_H

// Analog zu den Texturen werden hier die 3DS Modelle definiert (Pfade und Array-Indizes)

#define OBJECT3DS_COUNT					3

#define C5CARGO_OBJECT3DS_IDX			0
#define C5CARGO_OBJECT3DS_NAME			"data/c5_cargo.3ds"
#define F18_OBJECT3DS_IDX				1
#define F18_OBJECT3DS_NAME				"data/f18.3ds"
#define MISSILE_OBJECT3DS_IDX			2
#define MISSILE_OBJECT3DS_NAME			"data/generic_missile.3ds"

// Struktur für die 3DS-Objekte
typedef struct  {
	char		name[MAX_PATH_LENGTH];		// Pfadname des Modells
	Object3ds  *object;						// Das 3DS-Objekt
	int			id;							// ID des Objekts (wird nicht benutzt)
} object3ds_t;


#endif /*_3DSMODELLS_H */