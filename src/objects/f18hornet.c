//////////////////////////////////////////////////////////////////////////
// Laden und anpassen des 3D-Objekts F18hornet

#include "../stdafx.h"
#include "../3ds/3dsloader.h"
#include "../3ds/3dsmodells.h"
#include "f18hornet.h"

static texcoords *texcoords_array = NULL;
static vertexcoords *vertex_array = NULL;
static normalcoords *normal_array = NULL;

// Anzahl der Verticies (wird für das Rendern per Vertex-Array benötigt)
int num_verticies = 0;

//
// Laden des Objekts und verwenden eines Vertex-Arrays
//
GLvoid initF18Object()
{
	float min[3],max[3];
	
	// Laden des Objekts
	float *vertex = Load3DS(F18_OBJECT3DS_NAME, &num_verticies);

	// Prüfen ob das Objekt geladen werden konnte
	if(!vertex)
		devconout("... failed\n");
	else
	{
		devconout("... success\n");

		// Größe des Meshs ermitteln
		mesh_size(vertex,num_verticies,min,max);
		
		devconout("-------------------------------------------\n");
		devconout("num face %d\n",num_verticies / 3);
		devconout("min %f %f %f\n",min[0],min[1],min[2]);
		devconout("max %f %f %f\n",max[0],max[1],max[2]);
		devconout("-------------------------------------------\n");
		
		// Translieren
		mesh_translate(vertex,num_verticies,-(min[0] + max[0]) / 5.0f,
			-(min[1] + max[1]) / 5.0f,-(min[2] + max[2]) / 5.0f);
		// Skalieren
		mesh_scale(vertex,num_verticies,F18_MESH_RADIUS / mesh_radius(vertex,num_verticies));

	
		// Speicher für das Objekt allokieren.
		// Dieser wird für das Vertex-Array benötigt.
		texcoords_array = (texcoords *)malloc(num_verticies * sizeof(texcoords));
		vertex_array = (vertexcoords *)malloc(num_verticies * sizeof(vertexcoords));
		normal_array = (normalcoords *)malloc(num_verticies * sizeof(normalcoords));

		assert( (texcoords_array != NULL || vertex_array != NULL || normal_array != NULL) &&
				"initF18Object: memory allocation failed.");

		devconout("Building vertex array for f18object ... ");
		devconout("t = %d Kb", (sizeof(texcoords) * num_verticies) / 1024);
		devconout(", v = %d Kb", (sizeof(vertexcoords) * num_verticies) / 1024);
		devconout(", n = %d Kb\n", (sizeof(normalcoords) * num_verticies) / 1024);
		//glBegin(GL_TRIANGLES);

		for(int i = 0; i < num_verticies; i++) {
			// Der Bitshift sorgt dafür, dass um 8 Stellen im Array weiter-
			// gesprungen wird.
			int idx = i << 3;
			//float *ptr = &vertex[idx];	 
			
			// Texturkoordinaten im entsprechenden Array eintragen
			texcoords_array[i].s = vertex[idx + 6];
			texcoords_array[i].t = vertex[idx + 7];

			// Normalkoordinaten eintragen
			normal_array[i].x = vertex[idx + 3];
			normal_array[i].y = vertex[idx + 4];
			normal_array[i].z = vertex[idx + 5];

			// Verticies eintragen
			vertex_array[i].x = vertex[idx];
			vertex_array[i].y = vertex[idx + 1];
			vertex_array[i].z = vertex[idx + 2];

			//glTexCoord2fv(ptr + 6);
			//glNormal3fv(ptr + 3);
			//glVertex3fv(ptr);
		}
		//glEnd();

		// Speicher der vom 3DS-Loader für die Verticies angefordert wurde
		// kann nun wieder freigegeben werden

		free(vertex);


		// Vertex-Array vorbereiten
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
	} // if!vertex

	GL_ErrorReport();
}

//
//	Rendern des Objekts im Array.
//  TODO: Müsste profiliert werden um zu ermitteln ob die State-Switches inperformant sind...
//
GLvoid renderF18Object()
{
	// Arrays auf Gültigkeit prüfen
	assert((vertex_array || normal_array || texcoords_array) && 
		   "renderHeighmapVA: vertex arrays uninitialized.");

	glPushMatrix();
		// Da noch keine Texturierung verfügbar ist wird hier smooth shading verwendet
		glShadeModel(GL_SMOOTH);

		// Beleuchtung des Flugzeugs
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		//glEnable(GL_NORMALIZE);			// automatisches Normalisieren nach Skalierung
		float light[] = { 1, 300, 0, 0};
		//float ambient[] = {1,1,1,1};
		//glLightfv(GL_LIGHT0, GL_AMBIENT, light);
		glLightfv(GL_LIGHT0,GL_POSITION,light);
		
		// Flugzeug rotieren
		glRotatef(-90.0f,1,0,0);
		
		//////////////////////////////////////////////////////////////////////////
		// Draw the entire object.
		//////////////////////////////////////////////////////////////////////////
		
		// Load the data to each pointer type we need.
		// Das wird deshalb getan, da unterschiedliche Vertex-Arrays existieren können
		// glDrawArrays sorgt allerdings dafür, dass alle per gl...Pointer referenzierten
		// Arrays gerendert werden. Deshalb werden hier explizit die Pointer für dieses
		// Objekt angegeben
		glVertexPointer(3, GL_FLOAT, 0, vertex_array);
		glNormalPointer(GL_FLOAT, 0, normal_array);
		glTexCoordPointer(2, GL_FLOAT, 0, texcoords_array);

		// Die im Vertex-Array enthaltenen Verticies werden dem Profiler
		// gemeldet. Das deshalb, weil Vertex-Arrays nicht per glVertex3f() gerendert
		// werden und somit durch profile_glVertex3f() ersetzt werden könnten
		profile_addVertexCount(num_verticies);
		glDrawArrays(GL_TRIANGLES, 0, num_verticies);;

		/*glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);*/

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		//glDisable(GL_NORMALIZE);
		glShadeModel(GL_FLAT);
	glPopMatrix();
	
	GL_ErrorReport();
}

// Speicher bereinigen
GLvoid freeF18Object()
{
	if(texcoords_array)
		free(texcoords_array);
	if(normal_array)
		free(normal_array);
	if(vertex_array)
		free(vertex_array);
}

