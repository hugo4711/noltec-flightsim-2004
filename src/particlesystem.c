//////////////////////////////////////////////////////////////////////////
// Implementation eines Partikelsystems

// TODO: Funktion, die den Pool und den Emitter aufr�umt

#include "stdafx.h"
#include <strsafe.h>
#include "particlesystem.h"
#include "math/mathtoolkit.h"		// f�r die Zufallszahlen
#include "billboarding.h"

//
// Initialisieren des Partikelsystems.
// 
GLboolean InitParticleSystem(GLuint textureID,		   // TexturID f�r die Partikel. Wenn NULL, dann keine Textur
							 particlepool_t* pool,	   // Partikelpool in dem die Partikel im Ruhezustand vorhanden sind
							 emitter_t* em)			   // Emitter aus dem das Rendering stattfindet
{
	particle_t*	p = NULL; // Partikel das dem Pool hinzugef�gt wird
	
	// Zufallszahlen f�r dieses System sollen vom gleichen seed abh�ngen
	init_randseed_time();

	if(pool == NULL || em == NULL)
		return GL_FALSE;

	pool->head = NULL;
	pool->tail = NULL;
	pool->particlein = 0;
	pool->particleout = 0;

	// Initialisieren der Partikel und Hinzuf�gen zum Pool
	for(int i=0; i < MAXPARTICLES; i++)
	{
		// Initialisieren des Partikels
		p = (particle_t *)malloc(sizeof(particle_t));
		
		// Fehler?
		if(!p)
			return GL_FALSE;
		
		// Der Partikel wird im Emitter endg�ltig initialisiert
		// Hier ist es lediglich wichtig, die Zeigerreferenzen zu initialisieren
		p->next = NULL;
		p->prev = NULL;
		
		// Partikel zum Pool hinzuf�gen
		// Pr�fen, ob schon ein Element in der Liste vorhanden ist
		if(!(pool->head))
		{
			pool->head = p;				// Das neue Element am Kopf einf�gen
			pool->tail = p;				// und auch am Ende, weil es das Einzige ist.
			p->prev = NULL;				// Es gibt in diesem Fall keinen Vorg�nger oder Nachfolger
			p->next = NULL;
		}
		else
		{	// Anf�gen des neuen Elements am Ende der Liste
			pool->tail->next = p;		// Das bisher letzte Element zeigt nun auf das n�chste NEUE Element
			p->prev = pool->tail;		// Der Vorg�nger dieses Elements ist das alte Ende
			pool->tail		 = p;		// Das letzte Element der Liste ist das neue Element.
			p->next = NULL;				// Hier ist die Liste zu Ende
		}

		// Z�hler erh�hen
		pool->particlein++;
	}

	// Initialisieren des Emitters
	InitEmitter(em);

	return GL_TRUE;
}

// Speicher f�r die Partikel aus dem Pool und dem Emitter freigeben
void FreeParticlesFromSystem(particlepool_t *pool, emitter_t *em)
{
	assert(pool && em && "FreeParticlesFromSystem: invalid pool or emitter pointer.");

	// Alle Partikel aus dem Pool entfernen
	// falls vorhanden...
	particle_t *p;

	if(pool->head)
	{
		p = pool->head;

		while(p=p->next)
		{
			free(p->prev);
		}
		free(p);
	}
	
	// Alle Partikel aus dem Emitter entfernen
	// falls vorhanden...
	if(em->particlelist)
	{
		p = em->particlelist;

		while(p=p->next)
		{
			free(p->prev);
		}
		free(p);
	}
}

// Emitterwerte initialisieren
// TODO: Ausgliedern, da der Emitter entscheidend ist f�r das Verhalten des Partikelsystems und
//		 
void InitEmitter(emitter_t* em)
{
	em->id = 0;								// UNUSED
	StringCbCopyA(em->name, sizeof(em->name)-1,"Emitter");			// UNUSED
	em->flags = NULL;						// UNUSED
	
	em->position.x = 0.0f;					
	em->position.y = -0.5f;				
	em->position.z = -5.0f;	

	em->speed = 5.0f;						// Geschwindigkeit des Aussto�es
	em->delta_speed = 0.5f;					// �nderung der Geschwindigkeit

	em->particle_count = 0;					// Anzahl vorhandener Partikel im Emitter
	em->total_particles = MAXPARTICLES;		// Anzahl maximal vorhandener Partikel im Emitter
	em->emits_per_frame = 100;				// Aussto� pro Frame
	em->delta_emits = 10;					// �nderung des Aussto�es pro Frame

	em->startcolor.r = 0.8f;				// Startfarbe
	em->startcolor.g = 0.8f;
	em->startcolor.b = 0.8f;
	em->startcolor.a = 1.0f;
	
	em->delta_startcolor.r = 0.1f;
	em->delta_startcolor.g = 0.3f;
	em->delta_startcolor.b = 0.01f;
	em->delta_startcolor.a = 0.0f;

	em->endcolor.r = 1.0f;					// Endfarbe
	em->endcolor.g = 1.0f;
	em->endcolor.b = 1.0f;
	em->endcolor.a = 1.0f;

	em->delta_endcolor.r = 0.5f;
	em->delta_endcolor.g = 0.1f;
	em->delta_endcolor.b = 0.01f;
	em->delta_endcolor.a = 0.0f;



	em->lifetime = 20;
	em->delta_lifetime = 1;

	em->force.x = 0.0f;					// Richtung und St�rke der einwirkenden Kraft
	em->force.y = 0.0f;
	em->force.z = -5.0f;

	em->particlelist = NULL;				// NULL-Terminierte Liste
}

// 
// Neues Partikel zum Emitter em hinzuf�gen
// Zu diesem Zweck werden Partikel aus dem Pool geholt und zum Emitter hinzugef�gt
// Initialisiert das Partikel entsprechend vor (Farbe, Geschwindigkeit, Lebenszeit usw.)
//
GLboolean AddParticleToEmitter(particlepool_t* pool, emitter_t* em)
{
	particle_t* p; // Partikel aus dem Pool
	
	// Der Emitter die Gesamtzahl der Partikel noch nicht emittiert hat
	if (em && pool &&  // Pr�fen, ob Emitter und Pool g�ltig sind
		pool->head &&  // Pr�fen, ob der Pool �berhaupt noch Eintr�ge enth�lt
		em->particle_count < em->total_particles)
	{
		// Den letzten Partikel aus dem Pool nehmen und in
		// die Emitter-Liste stellen
		p = pool->tail;
		// Der letzte Eintrag im Pool ist jetzt der vorherige Partikel
		// falls der letzte Eintrag aber keinen Vorg�nger mehr hat, dann entspricht
		// dieser Eintrag gleichzeitig dem Ersten und tail muss gleich head sein
		if(pool->tail->prev)
			pool->tail = pool->tail->prev;
		else
			pool->tail = pool->head;

		pool->tail->next = NULL;				 // kein Nachfolger mehr.
		pool->particlein--;	pool->particleout++; // Poolz�hler aktualisieren


		// Partikel dem Emitter hinzuf�gen
		// TODO: Verstehen und kommentieren
		if (em->particlelist != NULL)
			em->particlelist->prev = p; 

		p->next = em->particlelist;	
		p->prev = NULL;				
		em->particlelist = p;		

		//////////////////////////////////////////////////////////////////////////
		// Partikelwerte initialisieren
		// Das geschieht z.T. zufallsbasiert
		// TODO: sollte ausgegliedert werden um differenzierte Partikelsysteme zu
		//		 erm�glichen
		//////////////////////////////////////////////////////////////////////////
		
		p->position.x = 0.0f;	// Position relativ zum Emitter
		p->position.y = 0.0f;
		p->position.z = 0.0f;

		p->prev_pos.x = 0.0f;	// Vorherige Position (f�r Antialiasing)
		p->prev_pos.y = 0.0f;
		p->prev_pos.z = 0.0f;

		p->direction.x = 0.1f * frandom_number();
		p->direction.y = 0.1f * frandom_number();
		p->direction.z = -0.0001f * frandom_number();

		// Geschwindigkeit bestimmen und auf den Richtungsvektor anwenden
		float speed = em->speed + (em->delta_speed * frandom_number());
		p->direction.x *= speed;
		p->direction.y *= speed;
		p->direction.z *= speed;

		// Farbwerte berechnen (Start und Ende)
		color4f_t start, end;
		start.r = em->startcolor.r + (em->delta_startcolor.r * frandom_number());
		start.g = em->startcolor.g + (em->delta_startcolor.g * frandom_number());
		start.b = em->startcolor.b + (em->delta_startcolor.b * frandom_number());
		start.a = em->startcolor.a + (em->delta_startcolor.a * frandom_number());
		
		end.r = em->endcolor.r + (em->delta_endcolor.r * frandom_number());
		end.g = em->endcolor.g + (em->delta_endcolor.g * frandom_number());
		end.b = em->endcolor.b + (em->delta_endcolor.b * frandom_number());
		end.a = em->endcolor.a + (em->delta_endcolor.a * frandom_number());

		p->color.r = start.r;
		p->color.g = start.g;
		p->color.b = start.b;
		p->color.a = start.a;

		// Lebenszeit berechnen
		p->lifetime = em->lifetime + em->delta_lifetime * (int)frandom_number();
		
		// Farb�nderung in Abh�ngigkeit der Lebenszeit bestimmen
		p->delta_color.r = (end.r - start.r) / p->lifetime;
		p->delta_color.g = (end.g - start.g) / p->lifetime;
		p->delta_color.b = (end.b - start.b) / p->lifetime;
		p->delta_color.a = (end.b - start.a) / p->lifetime;

		// Ein Partikel ist geboren
		em->particle_count++;
		return GL_TRUE;
	}
	return GL_FALSE;
}

//
// Eigenschaften des Partikels updaten. Wird von updateEmitter gerufen
//
GLboolean UpdateParticle(particle_t* p, particlepool_t* pool, emitter_t* em)
{
	// Pr�fem ob dieser Partikel ein G�ltiger ist,
	// bzw. ob die Lebenszeit abgelaufen ist
	if (p && p->lifetime > 0)
	{
		// Alte Partikelposition sichern (f�r Antialiasing)
		p->prev_pos.x = p->position.x;
		p->prev_pos.y = p->position.y;
		p->prev_pos.z = p->position.z;

		// Neue Position berechnen indem auf die Richtung zur�ckgegriffen wird
		p->position.x += p->direction.x;
		p->position.y += p->direction.y;
		p->position.z += p->direction.z;

		// Die einwirkende physialische Kraft anwenden
		p->direction.x += em->force.x;
		p->direction.y += em->force.y;
		p->direction.z += em->force.z;

		// Alte Farbe zwischenspeichern
		p->prev_color.r = p->color.r;
		p->prev_color.g = p->color.g;
		p->prev_color.b = p->color.b;
		p->prev_color.a = p->color.a;

		// Neue Farbe einstellen
		p->color.r += p->delta_color.r;
		p->color.g += p->delta_color.g;
		p->color.b += p->delta_color.b;
		p->color.a += p->delta_color.a;

		p->lifetime--;	// IT IS A CYCLE OLDER
		return GL_TRUE;
	}
	// Die Lebenszeit ist abgelaufen
	else if (p && p->lifetime == 0)
	{
		// Zur�ck in den Partikelpool mit diesem Partikel!
		// TODO: Verstehen was hier passiert
		
		// Referenzen in der Emitter-Liste entsprechend umbiegen:
		// pr�fen, ob p einen Vorg�nger besitzt
		if (p->prev) 
			p->prev->next = p->next; // Wenn p einen Vorg�nger hat, dann ist der Nachfolger
									 // von p der neue Nachfolger des Vorg�ngers von p
		else
			em->particlelist = p->next; // Wenn p keinen Vorg�nger hat, dann war es der Erste
										// in der Liste und der Nachfolger von p ist der neue Erste
		
		// Pr�fen, ob p einen Nachfolger besitzt
		if (p->next)
			p->next->prev = p->prev;    // Wenn p einen Nachfolger hat, dann ist der Vorg�nger
										// des Nachfolgers nicht mehr p, sondern p's Vorg�nger.
		em->particle_count--;			// Einer weniger im Emitter
		
		// p wieder zum Pool hinzuf�gen
		// TODO: evtl. hierf�r eine Funktion einf�hren, weil das dieselbe Prozedur ist,
		//		 wie beim Initialisieren des Pools

		// Pr�fen, ob schon ein Element in der Liste vorhanden ist
		if(!(pool->head))
		{
			pool->head = p;				// Das neue Element am Kopf einf�gen
			pool->tail = p;				// und auch am Ende, weil es das Einzige ist.
			p->prev = NULL;				// Es gibt in diesem Fall keinen Vorg�nger oder Nachfolger
			p->next = NULL;
		}
		else
		{	// Anf�gen des neuen Elements am Ende der Liste
			pool->tail->next = p;		// Das bisher letzte Element zeigt nun auf das n�chste NEUE Element
			p->prev = pool->tail;		// Der Vorg�nger dieses Elements ist das alte Ende
			pool->tail		 = p;		// Das letzte Element der Liste ist das neue Element.
			p->next = NULL;				// Hier ist die Liste zu Ende
		}

		pool->particlein++;pool->particleout--; // Anpassen der Z�hler
		
		return TRUE;
	}
	return GL_FALSE;
}

//
// Emitter updaten. Das geschieht einmal pro Frame. Ruft updateParticle auf.
// Erzeugt Partikel f�r den n�chsten Frame indem sie aus dem Pool geholt werden
//
GLboolean UpdateEmitter(particlepool_t* pool, emitter_t* em)
{
	particle_t* p;
	particle_structure* next; // Das hier muss sein, da es ansonsten Probleme mit next=p->next gibt
							  // siehe Definition von particle_t!
	
	// Wenn der Emitter existiert
	if(em)
	{
		// Wenn der Emitter Partikel in seiner Liste enth�lt
		if(em->particlelist)
		{
			// Die Partikelliste durchlaufen und alle Partikel updaten
			p = em->particlelist;
			while (p)
			{
				// Zwischenspeichern des n�chsten Partikels, weil 
				// es sein kann, das der aktuelle Partikel seine Lebenszeit
				// verliert
				next = p->next;	
				// Die Eigenschaften des Partikels updaten
				UpdateParticle(p, pool, em);
				p = next;
			} //while
		} //if

		// Die Partikel f�r den n�chsten Frame erzeugen
		int emit = em->emits_per_frame + em->delta_emits * (int)frandom_number();
		// Die Partikel aus dem Pool holen
		for (int i = 0; i < emit; i++)
			AddParticleToEmitter(pool, em);

		return GL_TRUE;
	}

	return GL_FALSE;
}

// Das Partikelsystem rendern. Wird einmal pro Frame ausgef�hrt
GLboolean RenderParticleSystem(emitter_t* em, GLboolean antialias, GLuint textureid)
{
	particle_t* p;
//	GLboolean antialias = GL_TRUE;

	// Wenn ein g�ltiger Emitter �bergeben wurde
	if (em)
	{
		// Wenn dieser Emitter eine g�ltige Partikelliste besitzt
		if (em->particlelist)
		{
			p = em->particlelist;
			
			//////////////////////////////////////////////////////////////////////////
			// Rendervorgang
			//////////////////////////////////////////////////////////////////////////
			
/*			glPushMatrix();
			static phi = 0.0f;

			if(++phi > 360.0f)
				phi = 0.0f;

			glRotatef(phi, 0.0f, 1.0f, 1.0f);
*/
			/*if (antialias)
				glBegin(GL_LINES);
			else
				glBegin(GL_POINTS);
			*/
			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFunc(GL_ONE, GL_ONE);
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);
			
			glBindTexture(GL_TEXTURE_2D, textureid);
			glEnable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);

			//////////////////////////////////////////////////////////////////////////
			// Alle Partikel aus dem Emmiter zeichnen
			//////////////////////////////////////////////////////////////////////////

			while (p)
			{
				/*if (antialias)
				{
					glColor4f(p->prev_color.r, p->prev_color.g, p->prev_color.b, p->prev_color.a);
					profile_glVertex3f(p->prev_pos.x,p->prev_pos.y,p->prev_pos.z);
				}
				glColor4f(p->color.r, p->color.g, p->color.b,p->color.a);
				profile_glVertex3f(p->position.x,p->position.y,p->position.z);
				*/

				//glColor4f(p->color.r, p->color.g, p->color.b,p->color.a);
				
				glTexCoord2f(0.0f, 0.0f); 
				profile_glVertex3f(p->position.x-0.5f, p->position.y-0.5f, p->position.z);  // unten links 
				glTexCoord2f(1.0f, 0.0f); 
				profile_glVertex3f(p->position.x+0.5f, p->position.y-0.5f, p->position.z);  // unten rechts
				glTexCoord2f(1.0f, 1.0f); 
				profile_glVertex3f(p->position.x+0.5f, p->position.y+0.5f, p->position.z);  // oben rechts
				glTexCoord2f(0.0f, 1.0f); 
				profile_glVertex3f(p->position.x-0.5f, p->position.y+0.5f, p->position.z);  // oben links

				p = p->next;

			}

			glEnd();
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);

			glColor3f(1.0f,1.0f,1.0f); // auf wei� resetten
			glPopMatrix();
		}
		GL_ErrorReport();
		return TRUE;
	}
	GL_ErrorReport();
	return FALSE;
}
