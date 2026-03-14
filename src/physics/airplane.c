//////////////////////////////////////////////////////////////////////////
// Implementationen des physikalischen Flugverhaltens eines Flugzeugs

// TODO: Lift und Drag Koeffizienten m�ssen aus externer Konfigurationsdatei geladen werden k�nnen!

#include "../stdafx.h"
#include "Airplane.h"

//
// Initialisieren der Flugzeugeigenschaften beim Start der Simulation
//
//
void InitAirplane(rigidbody_t *plane)
{
	float roll, pitch, yaw;	//rollen, neigen, gieren

	// Anfangsposition des Flugzeugs festlegen
	plane->position.x = 0.0f;
	plane->position.y = 0.0f;
	plane->position.z = 500.0f;

	// Anfangsbeschleunigung des Flugzeugs
	plane->velocity.x = 250.0f;
	plane->velocity.y = 0.0f;
	plane->velocity.z = 0.0f;
	plane->speed = 250.0f;

	// Anfangswinkelbeschleunigung festlegen
	plane->angular_velocity.x = 0.0f;
	plane->angular_velocity.y = 0.0f;
	plane->angular_velocity.z = 0.0f;

	// Schub, Krfte und Momente
	plane->forces.x = 2000.0f;
	plane->forces.y = 0.0f;
	plane->forces.z = 0.0f;
	plane->thrust_force = 2000.0f;

	// K�rperbeschleunigung initialisieren
	plane->velocity_body.x = 0.0f;
	plane->velocity_body.y = 0.0f;
	plane->velocity_body.z = 0.0f;

	// Klappenstatus und Str�mungsabriss initialisieren (wird sp�ter per Tastendruck/Berechnung)
	// entsprechend gesetzt
	plane->stall = false;
	plane->flaps = false;

	// Ausrichtung der Flugzeugachsen initialisieren
	roll = 0.0f;
	pitch = 0.0f;
	yaw = 0.0f;
	plane->orientation = MakeQuatFromEulerAngles(roll, pitch, yaw);

	// Now go ahead and calculate the plane's mass properties
	CalcAirplaneMassProperties(plane);

}

//
// Physikalische Kr�fte des Flugzeugs bestimmen (Masse, Tr�gheit, Gravitationszentrum,...)
// Wird einmalig zum Initialisieren ben�tigt.
//
//		Element 1:	Outboard, Port (left) wing section fitted with ailerons
//		Element 2:  Inboard, Port wing section fitted with landing flaps
//		Element 3:  Inboard, Starboard (right) wing section fitted with landing flaps
//		Element 4:	Outboard, Starboard wing section fitted with ailerons
//		Element 5:	Port elevator fitted with flap
//		Element 6:	Starboard elevator fitted with flap
//		Element 7:	Vertical tail/rudder (no flap the whole thing rotates)
//		Element 8:  The fuselage
//
void CalcAirplaneMassProperties(rigidbody_t *plane)
{
	vector3f_t	CG;
	float		Ixx, Iyy, Izz, Ixy, Ixz, Iyz;
	float		in, di;
	int         i;

	// Die einzelenen Elemente des Flugzeugs werden hier initialisiert, basierend auf einem
	// Design-Koordinatensystem. Sp�ter werden die Koordinaten so angepasst,
	// das sie sich auf das kombinierte Gravitationszentrum (das berechnet werden soll) beziehen
	
	plane->element[0].mass = 6.56f;
	plane->element[0].d_coords = NewVector(12.0f,12.0f,2.5f);
	plane->element[0].local_inertia = NewVector(13.92f,10.50f,24.00f);
	plane->element[0].incidence = -3.5f;
	plane->element[0].dihedral = 0.0f;
	plane->element[0].area = 31.2f;
	plane->element[0].flap = 0;

	plane->element[1].mass = 7.31f;
	plane->element[1].d_coords = NewVector(12.0f,5.5f,2.5f);
	plane->element[1].local_inertia = NewVector(21.95f,12.22f,33.67f);
	plane->element[1].incidence = -3.5f;
	plane->element[1].dihedral = 0.0f;
	plane->element[1].area = 36.4f;
	plane->element[1].flap = 0;

	plane->element[2].mass = 7.31f;
	plane->element[2].d_coords = NewVector(12.0f,-5.5f,2.5f);
	plane->element[2].local_inertia = NewVector(21.95f,12.22f,33.67f);
	plane->element[2].incidence = -3.5f;
	plane->element[2].dihedral = 0.0f;
	plane->element[2].area = 36.4f;
	plane->element[2].flap = 0;

	plane->element[3].mass = 6.56f;
	plane->element[3].d_coords = NewVector(12.0f,-12.0f,2.5f);
	plane->element[3].local_inertia = NewVector(13.92f,10.50f,24.00f);
	plane->element[3].incidence = -3.5f;
	plane->element[3].dihedral = 0.0f;
	plane->element[3].area = 31.2f;
	plane->element[3].flap = 0;

	plane->element[4].mass = 2.62f;
	plane->element[4].d_coords = NewVector(-10.0f,2.5f,3.0f);
	plane->element[4].local_inertia = NewVector(0.837f,0.385f,1.206f);
	plane->element[4].incidence = 0.0f;
	plane->element[4].dihedral = 0.0f;
	plane->element[4].area = 10.8f;
	plane->element[4].flap = 0;

	plane->element[5].mass = 2.62f;
	plane->element[5].d_coords = NewVector(-10.0f,-2.5f,3.0f);
	plane->element[5].local_inertia = NewVector(0.837f,0.385f,1.206f);
	plane->element[5].incidence = 0.0f;
	plane->element[5].dihedral = 0.0f;
	plane->element[5].area = 10.8f;
	plane->element[5].flap = 0;

	plane->element[6].mass = 2.93f;
	plane->element[6].d_coords = NewVector(-10.0f,0.0f,5.0f);
	plane->element[6].local_inertia = NewVector(1.262f,1.942f,0.718f);
	plane->element[6].incidence = 0.0f;
	plane->element[6].dihedral = 90.0f;
	plane->element[6].area = 12.0f;
	plane->element[6].flap = 0;

	plane->element[7].mass = 31.8f;
	plane->element[7].d_coords = NewVector(12.0f,0.0f,1.5f);
	plane->element[7].local_inertia = NewVector(66.30f,861.9f,861.9f);
	plane->element[7].incidence = 0.0f;
	plane->element[7].dihedral = 0.0f;
	plane->element[7].area = 84.0f;
	plane->element[7].flap = 0;

	// Normalvektoren f�r jede Fl�che berechnen.
	// Wird f�r dir relative Air-Speed bei der Auftrieb und Luftwiderstandsberechnung
	// ben�tigt
	for (int i = 0; i< PLANE_SUBSECTIONS; i++)
	{
		in = DEGTORAD(plane->element[i].incidence);
		di = DEGTORAD(plane->element[i].dihedral);
		plane->element[i].normal = NewVector((float)sin(in),
											(float)(cos(in)*sin(di)),
											(float)(cos(in)*cos(di)));
		VectNormalize(&plane->element[i].normal);
	}

	// Gesamtmasse des Flugzeugs berechnen (Summe der Einzelmassen)
	float mass = 0;
	for (i = 0; i< PLANE_SUBSECTIONS; i++)	
		mass += plane->element[i].mass;

	// Berechnen des Gravitationszentrums aus den einzelnen Momenten (moment = torque (Drehmoment)
	// Kraft die in einem bestimmten Winkel auf einen K�rper einwirkt (z.B. Newton-Meter)
	// torque = force * distance
	vector3f_t	moment = NewEmptyVector();
	
	// Gesamtmoment bestimmen
	// Moment f�r jede einzelne Sektion berechnen um das Gesamtmoment des Flugzeugs zu bestimmen.
	for (i = 0; i< PLANE_SUBSECTIONS; i++)
	    moment = AddVector(moment, VectScalarMult(plane->element[i].d_coords, plane->element[i].mass));

	// Gravitationszentrum berechnen (Gesamtmoment durch Gesamtmasse)
	CG = DivideVector(moment, mass);

	//  Koordinaten jedes Elements in Relation zum kombinierten Gravitationszentrum berechnen
	for (i = 0; i< PLANE_SUBSECTIONS; i++)
		plane->element[i].cg_coords = SubtractVector(plane->element[i].d_coords, CG);

	// Drehmoment und Tr�gheitsprodukt f�r die kombinierten Elemente berechnen
	// Die Tr�gheitsmatrix (Spannung) ist in K�rperkoordinaten abgebildet.
	// TODO: Was sagt diese Matrix (und ihre Inverse) aus?
	Ixx = 0;	Iyy = 0;	Izz = 0;
	Ixy = 0;	Ixz = 0;	Iyz = 0;
	for (i = 0; i< PLANE_SUBSECTIONS; i++)
	{
		Ixx += plane->element[i].local_inertia.x + plane->element[i].mass * (plane->element[i].cg_coords.y*plane->element[i].cg_coords.y + plane->element[i].cg_coords.z*plane->element[i].cg_coords.z);
		Iyy += plane->element[i].local_inertia.y + plane->element[i].mass * (plane->element[i].cg_coords.z*plane->element[i].cg_coords.z + plane->element[i].cg_coords.x*plane->element[i].cg_coords.x);
		Izz += plane->element[i].local_inertia.z + plane->element[i].mass * (plane->element[i].cg_coords.x*plane->element[i].cg_coords.x + plane->element[i].cg_coords.y*plane->element[i].cg_coords.y);
		Ixy += plane->element[i].mass * (plane->element[i].cg_coords.x * plane->element[i].cg_coords.y);
		Ixz += plane->element[i].mass * (plane->element[i].cg_coords.x * plane->element[i].cg_coords.z);
		Iyz += plane->element[i].mass * (plane->element[i].cg_coords.y * plane->element[i].cg_coords.z);
	}	

	// Da die Berechnungen beendet sind, k�nnen die Werte nun zugewiesen werden
	plane->mass = mass;
	// Tr�gheitsmatrix
	plane->inertia.a11 = Ixx;	plane->inertia.a12 = -Ixy;	plane->inertia.a13 = -Ixz;
	plane->inertia.a21 = -Ixy;	plane->inertia.a22 = Iyy;	plane->inertia.a23 = -Iyz;
	plane->inertia.a31 = -Ixz;	plane->inertia.a32 = -Iyz;	plane->inertia.a33 = Izz;
	// Inverse der Tr�gheitsmatrix bilden
	plane->inertia_inv = InverseMatrix(plane->inertia);
}

//
// Berechnen der auf das Flugzeug einwirkenden Kr�fte und Momente zu
// jeder gegebenen Zeit
//
void CalcAirplaneLoads(rigidbody_t *plane)
{
	// Kr�fte und Momente in K�rperkoordinaten berechnen
	vector3f_t	local_velocity = NewEmptyVector();
	float		local_speed = 0.0f;
	vector3f_t	drag_vector = NewEmptyVector();
	vector3f_t	lift_vector = NewEmptyVector();
	float		aoa = 0.0f;
	float		tmp = 0.0f;
	vector3f_t	resultant = NewEmptyVector();
	int			i;
	vector3f_t	vtmp = NewEmptyVector();
	vector3f_t	total_force = NewEmptyVector(); vector3f_t total_moment = NewEmptyVector();

	// Kr�fte und Momente zur�ckseteen
	plane->forces = NewEmptyVector();
	plane->moments = NewEmptyVector();
	total_force = NewEmptyVector();				/* Gesamtkraft */
	total_moment = NewEmptyVector();			/* Gesamtmoment */

	// Schubvektor definieren der hier durch das Gravitationszentrum des Flugzeugs verl�uft
	// In Relation zum K�rper gesehen.
	plane->thrust = NewVector(1.0f, 0.0f, 0.0f);
	// Schub berechenen
	plane->thrust = VectScalarMult(plane->thrust, plane->thrust_force);
	// Str�mungsabriss zur�cksetzen
	plane->stall = false;

	// �ber alle Elemente laufen (au�er den Rumpf)
	i = 0;
	while( i<PLANE_SUBSECTIONS) 
	{
		// Das H�henruder ist ein Spezialfall weil es rotieren kann
		// und daher der Normalvektor neu berechnet werden muss
		if (i == 6) 
		{           
			float in, di;
			in = DEGTORAD(plane->element[i].incidence);   // incidence angle
			di = DEGTORAD(plane->element[i].dihedral);	  // dihedral angle
			// Normalvektor der Oberfl�che
			plane->element[i].normal = NewVector((float)sin(in), 
												 (float)(cos(in)*sin(di)), 
												 (float)(cos(in)*cos(di)));
			// Normalisieren
			VectNormalize(&plane->element[i].normal);
		}

		// Lokale Beschleunigung berechenen (inkl. der Beschleunigung, die auf linearer
		// Bewegung des Flugzeugs beruht und die Beschleunigung an jedem Element die durch
		// Rotation des Flugzeugs hervorgerufen wird.
		
		// Rotationsteil
		vtmp = VectCrossProduct(plane->angular_velocity, plane->element[i].cg_coords); 
		// Gesamte lokale Beschleungigung = K�rperbeschleunigung + Rotationsteil
		local_velocity = AddVector(plane->velocity_body, vtmp); 

		// Lokale Air-Speed berechnen (L�nge des local_velocity Vektors
		local_speed = VectMagnitude(local_velocity);

		// Richtung des Luftwiderstands berechnen (wirkt immer schritthaltend mit der relativen 
		// Geschwindigkeit, aber in entgegengesetzter Richtung
		if(local_speed > 1.) 
			drag_vector = DivideVector(VectInvert(local_velocity), local_speed);

		// Richtung des Auftriebs bestimmen (lift)
		// Auftrieb ist immer Senkrecht zum Luftwiderstandsvektor (drag)
		lift_vector = VectCrossProduct(VectCrossProduct(drag_vector, plane->element[i].normal), drag_vector);
		tmp = VectMagnitude(lift_vector);
		// Normalisieren
		VectNormalize(&lift_vector);

		// Anstellwinkel (Angle of Attack (AoA) berechnen.
		// Der Anstellwinkel ist der Winkel zwische Auftriebsvektor (lift_vector) und
		// dem Normalvektor des Elements. Berechnung erfolgt durch das Skalarprodukt 
		// Anm.: Der Sinus des Anstellwinkels ist gleich
		// dem Cosinus des Winkels zwischen dem Luftwiderstandsvektor (drag_vector) und
		// dem Normalvektor
		tmp = VectDotProduct(drag_vector, plane->element[i].normal);
		if(tmp > 1.) tmp = 1;
		if(tmp < -1) tmp = -1;
		// Anstellwinkel bestimmen
		aoa = RADTODEG((float) asin(tmp));

		// Resultierenden Auftrieb und Luftwiderstand berechnen (Luftdichte ist als Konstante RHO
		// definiert)
		tmp = 0.5f * RHO * local_speed * local_speed * plane->element[i].area;		
		
		// Summe aus den Skalarprodukten des Auftriebsvektors und Luftwiderstandsvektors
		// * Anstellwinkel
		if (i == 6) // Sonderbehandlung f�r das H�henruder
		{
			resultant =	AddVector( VectScalarMult(lift_vector, RudderLiftCoefficient(aoa) ),
								   VectScalarMult(drag_vector, RudderDragCoefficient(aoa) ) );
			resultant = VectScalarMult(resultant, tmp);
		} 
		else if (i == 7) // Luftwiderstand des Rumpfes simulieren
		{
		 	 resultant = VectScalarMult(VectScalarMult(drag_vector, 0.5f), tmp); 
		}
		else		// Die anderen Fl�gelelemente
		{
			 resultant = AddVector( VectScalarMult( lift_vector, LiftCoefficient(aoa, plane->element[i].flap) ),
                   				    VectScalarMult( drag_vector, DragCoefficient(aoa, plane->element[i].flap) ) );
			 resultant = VectScalarMult(resultant, tmp);
		}
		
		// Str�mungsabriss pr�fen.
		// Das ist dann der Fall, wenn der Auftriebskoeffizient (lift coefficient (cl)) = 0 ist.
		// In der Realit�t gibt es Warnger�te die den Piloten auf einen Str�mungsabriss hinweisen.
		// Das geschieht noch bevor die kritische Schwelle erreicht wurde um dem Piloten eine Korrektur
		// zu erlauben.

		// Nur f�r die Hauptfl�gelfl�chen
		if (i<=3 && LiftCoefficient(aoa, plane->element[i].flap) == 0)
			plane->stall = true;			
		
		// Gesamtkraft bestimmen
		total_force = AddVector(total_force, resultant);

		// Moment um das Gravitationszentrum der auf dieses Element einwirkenden Kraft berechnen
		// und Gesamtmoment bestimmen
		vtmp = VectCrossProduct(plane->element[i].cg_coords, resultant);
		total_moment = AddVector(total_moment, vtmp);

		i++;
	}

	// Schub hinzuaddieren
	total_force = AddVector(total_force, plane->thrust);

	// K�rperkoordinaten in Erdkoordinaten der Gesamtkr�fte umrechnen
	plane->forces = QuatVectRotate(plane->orientation, total_force);

	// Gravitation hinzuf�gen (g = -32.174 ft/s^2)
	plane->forces.z += G_FORCE * plane->mass;

	// Gesamtmoment berechnen
	plane->moments = AddVector(plane->moments, total_moment);
}

//
// Extrahieren der z-Komponente der aktuellen Ausrichtung des Flugzeugs
//
vector3f_t GetBodyZAxisVector(rigidbody_t *plane)
{

	vector3f_t	v;

	v.x = 0.0f;
	v.y = 0.0f;
	v.z = 1.0f;

	return QuatVectRotate(plane->orientation, v);
}

//
// Extrahieren der x-Komponente der aktuellen Ausrichtung des Flugzeugs
//
vector3f_t	GetBodyXAxisVector(rigidbody_t *plane)
{

	vector3f_t v;

	v.x = 1.0f;
	v.y = 0.0f;
	v.z = 0.0f;

	return QuatVectRotate(plane->orientation, v);

}

//
// Bestimmen des Auftriebskoeffizienten anhand der Klappenstellung und einer Look-Up Table
// Die Auftriebswerte werden normalerweise im Windkanal f�r unterschiedliche Fl�geltypen
// ermittelt.
// Diese Daten sind f�r eine Standardfl�gelklasse geeignet.
//
// TODO: Diese Daten m�ssen aus einer externen Konfigurationsdatei eingelesen werden!
//
float	LiftCoefficient(float angle, float flaps)
{
	float clf0[9] = {-0.54f, -0.2f, 0.2f, 0.57f, 0.92f, 1.21f, 1.43f, 1.4f, 1.0f};
	float clfd[9] = {0.0f, 0.45f, 0.85f, 1.02f, 1.39f, 1.65f, 1.75f, 1.38f, 1.17f};
	float clfu[9] = {-0.74f, -0.4f, 0.0f, 0.27f, 0.63f, 0.92f, 1.03f, 1.1f, 0.78f};
	float a[9]	 = {-8.0f, -4.0f, 0.0f, 4.0f, 8.0f, 12.0f, 16.0f, 20.0f, 24.0f};
	float cl = 0;
	int	  i;

	for (i=0; i<8; i++)
	{
		if( (a[i] <= angle) && (a[i+1] > angle) )
		{
			float cl0 = clf0[i] - (a[i] - angle) * (clf0[i] - clf0[i+1]) / (a[i] - a[i+1]);
			if (flaps > 0) {
				float cl1 = clfu[i] - (a[i] - angle) * (clfu[i] - clfu[i+1]) / (a[i] - a[i+1]);
				cl = cl0 + (cl1 - cl0) * flaps;
			} else if (flaps < 0) {
				float cl_1 = clfd[i] - (a[i] - angle) * (clfd[i] - clfd[i+1]) / (a[i] - a[i+1]);
				cl = cl0 + (cl_1 - cl0) * (-flaps);
			} else {
				cl = cl0;
			}
			break;
		}
	}	
	return cl;
}

//
// Luftwiderstandskoeffizienten
//
//
float	DragCoefficient(float angle, float flaps)
{
	float cdf0[9] = {0.01f, 0.0074f, 0.004f, 0.009f, 0.013f, 0.023f, 0.05f, 0.12f, 0.21f};
	float cdfd[9] = {0.0065f, 0.0043f, 0.0055f, 0.0153f, 0.0221f, 0.0391f, 0.1f, 0.195f, 0.3f};
	float cdfu[9] = {0.005f, 0.0043f, 0.0055f, 0.02601f, 0.03757f, 0.06647f, 0.13f, 0.18f, 0.25f};
	float a[9]	 = {-8.0f, -4.0f, 0.0f, 4.0f, 8.0f, 12.0f, 16.0f, 20.0f, 24.0f};
	float cd = 0.75;
	int	  i;

	for (i=0; i<8; i++)
	{
		if( (a[i] <= angle) && (a[i+1] > angle) )
		{
			float cd0 = cdf0[i] - (a[i] - angle) * (cdf0[i] - cdf0[i+1]) / (a[i] - a[i+1]);
			if (flaps > 0) {
				float cd1 = cdfu[i] - (a[i] - angle) * (cdfu[i] - cdfu[i+1]) / (a[i] - a[i+1]);
				cd = cd0 + (cd1 - cd0) * flaps;
			} else if (flaps < 0) {
				float cd_1 = cdfd[i] - (a[i] - angle) * (cdfd[i] - cdfd[i+1]) / (a[i] - a[i+1]);
				cd = cd0 + (cd_1 - cd0) * (-flaps);
			} else {
				cd = cd0;
			}
			break;
		}
	}	
	return cd;
}

//
// Auftriebskoeffizienten für das Heckruder
//
float	RudderLiftCoefficient(float angle)
{
	float clf0[7] = {0.0f, 0.456f, 0.736f, 0.968f, 1.144f, 1.12f, 0.8f};
	float a[7]	 = {0.0f, 4.0f, 8.0f, 12.0f, 16.0f, 20.0f, 24.0f};
	float cl;
	int	  i;
	float	aa = (float) fabs(angle);

	cl = 0;
	for (i=0; i<6; i++)
	{
		if( (a[i] <= aa) && (a[i+1] > aa) )
		{			
			cl = clf0[i] - (a[i] - aa) * (clf0[i] - clf0[i+1]) / (a[i] - a[i+1]);
			if (angle < 0) cl = -cl;
			break;
		}
	}	
	return cl;
}

//
// Luftwiderstandskoeffizienten f�r das H�henruder
//
//
float	RudderDragCoefficient(float angle)
{
	float cdf0[7] = {0.0032f, 0.0072f, 0.0104f, 0.0184f, 0.04f, 0.096f, 0.168f};
	float a[7]	 = {0.0f, 4.0f, 8.0f, 12.0f, 16.0f, 20.0f, 24.0f};
	float cd;
	int	  i;
	float	aa = (float) fabs(angle);

	cd = 0.75;
	for (i=0; i<6; i++)
	{
		if( (a[i] <= aa) && (a[i+1] > aa) )
		{			
			cd = cdf0[i] - (a[i] - aa) * (cdf0[i] - cdf0[i+1]) / (a[i] - a[i+1]);			
			break;
		}
	}	
	return cd;
}

//
// Echtzeitberechnung nach der Euler-Methode
//
void StepSimulationStdEuler(float delta_time, rigidbody_t *plane)
{
	// Take care of translation first:
	// (If this body were a particle, this is all you would need to do.)
	vector3f_t Ae;	

	// calculate all of the forces and moments on the airplane:
	CalcAirplaneLoads(plane);

	// Cap angular_velocity components to +/- 5.0
	if (plane->angular_velocity.x > 10.0f) plane->angular_velocity.x = 10.0f;
	if (plane->angular_velocity.x < -10.0f) plane->angular_velocity.x = -10.0f;
	if (plane->angular_velocity.y > 10.0f) plane->angular_velocity.y = 10.0f;
	if (plane->angular_velocity.y < -10.0f) plane->angular_velocity.y = -10.0f;
	if (plane->angular_velocity.z > 10.0f) plane->angular_velocity.z = 10.0f;
	if (plane->angular_velocity.z < -10.0f) plane->angular_velocity.z = -10.0f;

	// Cap velocity magnitude to 2000.0
	float vel_mag = VectMagnitude(plane->velocity);
	if (vel_mag > 2000.0f) {
		plane->velocity = VectScalarMult(DivideVector(plane->velocity, vel_mag), 2000.0f);
	}

	// Beschleunigung in Erdkoordinaten berechnen
	Ae = DivideVector(plane->forces, plane->mass);

	// Geschwindigkeit in Erdkoordinaten berechnen
	plane->velocity = AddVector(plane->velocity,  VectScalarMult(Ae, delta_time));

	// Position des Flugzeugs in Erdkoordinaten berechnen
	plane->position = AddVector(plane->position, VectScalarMult(plane->velocity, delta_time));

	// Rotationen handeln
	float		mag;

	// Winkelbeschleunigung des Flugzeugs in Erdkoordinaten berechnen
	// Das ist hier ein wenig un�bersichtlich geraten. Die Formel lautet:
	// inertia_inv * (moments - (angular_velocity x (inertia * angular_velocity))) * delta_time;
	vector3f_t vtmp = NewEmptyVector();
	vtmp = SubtractVector(plane->moments,
					VectCrossProduct(plane->angular_velocity,
									 MatrixMultVector(plane->inertia, plane->angular_velocity) ) );
	plane->angular_velocity = AddVector(plane->angular_velocity, VectScalarMult(MatrixMultVector(plane->inertia_inv, vtmp),
											 delta_time));

	// Neues Rotationsquaternion berechnen
	quaternion_t qtmp = NewEmptyQuaternion();
	qtmp = MultiplyQuatVector(plane->orientation, plane->angular_velocity);
	plane->orientation = AddQuat(plane->orientation, MultiplyQuatScalar(qtmp, (0.5f * delta_time)));


	// Ausrichtungsquaternion normalisieren
	mag = QuatMagnitude(plane->orientation);
	if (mag < 0.0001f || mag > 10.0f) {
		QuatNormalize(&plane->orientation);
	} else if (mag != 0) {
		QuatNormalize(&plane->orientation);
	}

	// Geschwindigkeit in K�rperkoordinaten berechnen
	// Dies wird f�r die Auftriebs- und Luftwiderstandskraftberechnung ben�tigt
	plane->velocity_body = QuatVectRotate(ConjugateQuat(plane->orientation), plane->velocity);

	// Air-Speed berechnen
	plane->speed = VectMagnitude(plane->velocity);

	// Euler Winkel erzuegen
	vector3f_t u;

	u = MakeEulerAnglesFromQuat(plane->orientation);
	plane->euler_angle.x = u.x; // roll
	plane->euler_angle.y = u.y; // pitch
	plane->euler_angle.z = u.z; // yaw

	// NaN protection
	if (plane->position.x != plane->position.x || plane->position.y != plane->position.y || plane->position.z != plane->position.z ||
		plane->orientation.w != plane->orientation.w || plane->orientation.v.x != plane->orientation.v.x || 
		plane->orientation.v.y != plane->orientation.v.y || plane->orientation.v.z != plane->orientation.v.z) {
		InitAirplane(plane);
	}

}

// 
// Tastatureingaben: Schub erh�hen
//
void IncThrust(rigidbody_t *plane)
{
	plane->thrust_force += DTHRUST;
	if(plane->thrust_force > MAXTHRUST)
		plane->thrust_force = MAXTHRUST;
}

//
//Tastatureingaben: Schub verringern
//
void DecThrust(rigidbody_t *plane)
{
	plane->thrust_force -= DTHRUST;
	if(plane->thrust_force < 0)
		plane->thrust_force = 0;
}
// Pitch up
void PitchUp(rigidbody_t *plane)
{
	plane->element[4].flap = 0.3;
	plane->element[5].flap = 0.3;
}

void PitchDown(rigidbody_t *plane)
{
	plane->element[4].flap = -0.3;
	plane->element[5].flap = -0.3;
}

void RollLeft(rigidbody_t *plane)
{
	plane->element[0].flap = 0.3;
	plane->element[3].flap = -0.3;
}

void RollRight(rigidbody_t *plane)
{
	plane->element[0].flap = -0.3;
	plane->element[3].flap = 0.3;
}


void ZeroRudder(rigidbody_t *plane)
{
	plane->element[6].incidence = 0;
}

void ZeroAilerons(rigidbody_t *plane)
{
	plane->element[0].flap = 0;
	plane->element[3].flap = 0;
}

void ZeroElevators(rigidbody_t *plane)
{
	plane->element[4].flap = 0;
	plane->element[5].flap = 0;
}