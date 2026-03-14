//////////////////////////////////////////////////////////////////////////
// Definitionen für starre Körper (physikalisches Verhalten)


#ifndef _AIRPLANE_H
#define _AIRPLANE_H

//------------------------------------------------------------------------//
// Notes:
//------------------------------------------------------------------------//
/* 

Earth coordinates:	
x points North
y points West
z points up


			Z   X
			|  /
			| /
			|/
	Y------+


Body coordinates:	
x points to the front	
y points to left
z points up 


Units:	English system,			
distance	--> feet
time		-->	seconds
mass		--> slugs
force		--> pounds
moment		--> foot-pounds
velocity	--> feet-per-second
acceleration-->	feet-per-second^2
density		--> slugs-per-feet^3

*/



#include "../math/quaternion.h"
#include "../math/vectormath.h"
#include "../math/matrix3x3.h"

#define PLANE_SUBSECTIONS	8
#define G_FORCE			  -32.174f			// Gravitationsbeschleunigung ft/s^2
#define RHO				    0.0023769001f	// Luftdichte auf Seehöhe slugs/ft^3
#define DTHRUST				  10.0f			// Schubänderung bei Tastendruck
#define MAXTHRUST			1000.0f			// Maximaler Schub

// Ein Strukturelles Elemet des Flugzeugs
typedef struct {
	float		mass;			// Die masse des Elements
	vector3f_t	d_coords;		// Drag Vektor
	vector3f_t	cg_coords;		// CG Vektor (Center of Gravity)
	vector3f_t	local_inertia;	// Trägheitsmoment
	float		incidence;		// Einfallswinkel in Relation zum CG
	float		dihedral;		// Öffnungswinkel (gibt die Relation zu x-Achse an.
	// bei normalen Flügeln immer 0, außer beim tail-rudder
	vector3f_t	normal;			// Normalvektor des Elements in Relation zum CG
	float		area;			// Gesamtfläche des Elements
	float			flap;			// Klappenstellung (falls vorhanden)
} bodyelement_t;

typedef struct {
	float		mass;				// Masse
	matrix3x3_t inertia;			// Trägheit
	matrix3x3_t inertia_inv;		// Inverse der Trägheit
	vector3f_t	position;			// Position in Erdkoordinaten
	vector3f_t	velocity;			// Geschwindigkeit in Erdkoordinaten
	vector3f_t	velocity_body;		// Geschwindigkeit in Körperkoordinaten
	vector3f_t	angular_velocity;	// Winkelgeschwindigkeit in Körperkoordinaten
	vector3f_t	euler_angle;		// Eulerwinkel in Körperkoordinaten
	float		speed;				// Tempo = Betrag der Beschleunigung
	quaternion_t orientation;		// Ausrichtung in Erdkoordinaten
	vector3f_t	forces;				// Gesamtkraft auf den Körper
	vector3f_t	moments;			// Gesamtmoment auf den Körper

	// TODO: Das müsste irgendwie ausgegliedert werden aber in C ist ableiten nicht möglich
	//		 daher wird dem RigidBory die Eigenschaften eines Flugzeugs hinzugefügt...
	bodyelement_t element[PLANE_SUBSECTIONS];	// Einzelne Elemente des Flugzeugs die berechnet werden müssen
	vector3f_t  thrust;				// Schubvektor von dem angenommen wird, dass er durch das CG verläuft
	float		thrust_force;		// Schub, Betrag des Thrust-Vektors
	bool		flaps;				// Klappenstellung
	bool		stall;				// Strömungsabriss?
} rigidbody_t;


void InitAirplane(rigidbody_t *plane);
void CalcAirplaneMassProperties(rigidbody_t *plane);
float	LiftCoefficient(float angle, float flaps);
float	DragCoefficient(float angle, float flaps);
float	RudderLiftCoefficient(float angle);
float	RudderDragCoefficient(float angle);

vector3f_t	GetBodyZAxisVector(rigidbody_t *plane);
vector3f_t	GetBodyXAxisVector(rigidbody_t *plane);

// Tastatureingaben verarbeiten
void IncThrust(rigidbody_t *plane);
void DecThrust(rigidbody_t *plane);
void PitchUp(rigidbody_t *plane);
void	PitchDown(rigidbody_t *plane);
void RollLeft(rigidbody_t *plane);
void RollRight(rigidbody_t *plane);
void ZeroRudder(rigidbody_t *plane);
void ZeroAilerons(rigidbody_t *plane);
void ZeroElevators(rigidbody_t *plane);
// Echtzeitberechnung

// Standard Euler-Methode
void StepSimulationStdEuler(float delta_time, rigidbody_t *plane);


#endif  _AIRPLANE_H
