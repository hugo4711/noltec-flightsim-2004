//////////////////////////////////////////////////////////////////////////
// Implementationen des Mathtoolkits

#include "../stdafx.h"
#include "mathtoolkit.h"

// kleine, zufällige Fließkommazahl generieren
float frandom_number()
{
	return ((float)(rand() - HALF_RAND) / (float)HALF_RAND);
}

// seed für die Zufallszahlen initialisieren. Hier basierend auf der aktuellen Systemzeit in s.
void init_randseed_time()
{
	// Zufallszahlengenerator initialisieren
	srand((unsigned)time(NULL));
}


// Lookup-Tables zur Effizienzsteigerung für sin/cos
// Macht nur bei "groben" Rotationen in vollen Grad-Schritten Sinn!
float *cos_lookup;
float *sin_lookup;

void build_lookuptables_sincos()
{
	sin_lookup = (float *)calloc(360, sizeof(float));
	cos_lookup = (float *)calloc(360, sizeof(float));

	assert(sin_lookup && cos_lookup && "build_lookuptables_sincos: memory allocation failed.");

	for(int angle = 0; angle < 360; angle++)
	{
		sin_lookup[angle] = sinf(DEGTORAD((float)angle));
		cos_lookup[angle] = cosf(DEGTORAD((float)angle));
	}

}


/*
** Direkt aus dem Wolfenstein Enemy Territory Code:
** Schnele Quadratwurzelberechnung (geht ursprünglich aus Quake 2 hervor)
** Stichwort: Newton/Rhapson Iteration
** -> Ergebnis muss mit 10.000 multipliziert werden. Das bremst noch die Performance.
** muss noch gefixt werden
** float q_rsqrt( float number )
*/
float q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;						// evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the ****?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}