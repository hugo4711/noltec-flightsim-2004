#ifndef _MATHTOOLKIT_H
#define _MATHTOOLKIT_H

// Mathematische Definitionen aus der math.h verwenden
#include <math.h>

#define FLOAT_TYPE_TOLERANCE		0.0001f			// Toleranzgrenze für Normalisieren
#define PI							3.14159265358979323846f

// Makrodefinitionen
#define DEGTORAD(x)					x * PI / 180.0f 
#define RADTODEG(x)					x * 180.0f / PI

// Für die Generierung von Zufallszahlen
#define HALF_RAND					RAND_MAX / 2

// kleine Zufallszahlen generieren
float frandom_number();
// seed initialisieren
void init_randseed_time();

// lookup-tables
extern float *cos_lookup;
extern float *sin_lookup;
void build_lookuptables_sincos();

// Schnelle Quadratwurzelberechnung
float q_rsqrt( float number );

#endif /* _MATHTOOLKIT_H */


