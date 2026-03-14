//////////////////////////////////////////////////////////////////////////
// Das Partikelsystem

#ifndef _PARTICLESYSTEM_H
#define _PARTICLESYSTEM_H

#include "math/vectormath.h"

#define MAXPARTICLES			2000

// RGBA Farben
typedef struct 
{
	float r;		// red
	float g;		// green
	float b;		// blue
	float a;		// alpha
} color4f_t;

// Ein Partikel
typedef struct particle_structure
{
	particle_structure	*prev, *next;	// Doppelte Verkettung
	vector3f_t	position;				// Aktuelle Position des Partikels
	vector3f_t	prev_pos;				// Vorherige Position
	vector3f_t	direction;				// Bewegungsrichtung
	int			lifetime;				// Lebenszeit des Partikels
	float		size;					// Grüße des Partikels
	color4f_t	color;					// Farbe des Partikels
	color4f_t   prev_color;				// Vorherige Farbe des Partikels
	color4f_t	delta_color;			// Farbänderung
	GLuint		textureID;				// Die dem Partikel zugeordnete Textur
} particle_t;


// Der Partikelpool besteht aus mehreren Partikeln
typedef struct 
{
	particle_t* head;
	particle_t* tail;
	int particlein;			// Gibt an, wieviele Partikel in der Liste sind
	int particleout;		// Gibt an, wieviele Partikel im Emitter sind
} particlepool_t;

// Partikel-Emitter
typedef struct 
{
	long		id;					// Emitter ID
	char		name[80];			// Bezeichner
	long		flags;				// Flags
	// Transformationsinformationen
	vector3f_t  position;			// Position an der die Partikel emittiert werden sollen
	vector3f_t  prevpos;			// Vorherige Position des Partikels (für Antialiasing)
	float		speed;				// Geschwindigkeit
	float		delta_speed;		// Geschwindigkeitsänderung
	// Partikelinformationen
	particle_t  *particlelist;			// Verkettete Liste, NULL Terminiert
	int			total_particles;	// Gesamtanzahl emittierter Partikel
	int			particle_count;		// Anzahl der Partikel im Emitter
	int			emits_per_frame;	// Anzahl emittierter Partikel pro Frame
	int			delta_emits;		// Veränderung emittierter Partikel
	int			lifetime;			// Lebenszeit
	int			delta_lifetime;		// Veränderung der Lebenszeit
	color4f_t	startcolor;			// Anfangsfarbe
	color4f_t	endcolor;			// Endfarbe
	color4f_t   delta_startcolor;	// Farbänderung der Startfarbe
	color4f_t	delta_endcolor;		// Farbänderung der Endfarbe
	// Physikalische Einwirkungen
	vector3f_t	force;				// Physikalisch einwirkende Kraft (Wind, etc.)
} emitter_t;

// Partikelsystem initialisieren
GLboolean InitParticleSystem(GLuint textureID, particlepool_t* pool, emitter_t* em);
// Initialisieren des Emitters
void InitEmitter(emitter_t* em);
// Speicher für im Pool UND im Emitter befindliche Partikel freigeben
void FreeParticlesFromSystem(particlepool_t *pool, emitter_t *em);
// Partikel zum Emitter hinzufügen, nur diese Werden später gezeichnet. Gleichzeitig
// wird der Partikel aus dem Pool entfernt
GLboolean AddParticleToEmitter(particlepool_t* pool, emitter_t* em);
// Emitter einmal pro Frame updaten
GLboolean UpdateEmitter(particlepool_t* pool, emitter_t* em);
// Partikel im Emitter updaten
GLboolean UpdateParticle(particle_t* p, particlepool_t* pool, emitter_t* em);
// Partikelsystem rendern
GLboolean RenderParticleSystem(emitter_t* em , GLboolean antialias, GLuint textureid);


#endif /* _PARTICLESYSTEM_H */