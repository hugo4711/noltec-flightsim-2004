//////////////////////////////////////////////////////////////////////////
// Heightmap Implementation

#include "../stdafx.h"
#include "geomap.h"
#include "../billboarding.h"	// f�r das Treemapping
#include "../frustum.h"			// Frustum-Culling

//
// NAME: DrawHeightMapShaded
//
// ZWECK: H�henkarte anhand der �bergebenen RAW-Bitmap rendern
//
// KOMMENTAR: Zeichnet eine Hight-Map mit Schattierungen, ohne Texturen, indem
//			  die Fkt. SetVertexColor() benutzt wird.
//
void DrawHeightMapShaded(BYTE *pHeightMap)
{
	int X = 0, Y = 0;						// Create some variables to walk the array with.
	int x, y, z;							// Create some variables for readability

	if(!pHeightMap) return;					// Make sure our height data is valid

	glBegin( GL_QUADS );					// Render Quads

	// Next we actually need to draw the terrain from the height map.
	// To do that, we just walk the array of height data and pluck out
	// some heights to plot our points.  If we could see this happening,
	// it would draw the columns first (Y), then draw the rows.
	// Notice that we have a MAP_RESOLUTION.  This determines how defined our
	// height map is.  The higher the MAP_RESOLUTION, the more blocky the terrain
	// looks, while the lower it gets, the more rounded it becomes.
	// If we set MAP_RESOLUTION = 1 it would create a vertex for every pixel in the height map.
	// I chose 16 as a decent size.  Anything too much less gets to be insane and slow.
	// Of course, you can increase the number when you get lighting in.
	// Then vertex lighting would cover up the blocky shape.  Instead of lighting,
	// we just put a color value associated with every poly to simplify the tutorial.
	// The higher the polygon, the brighter the color is.

	for ( X = 0; X < HEIGHTMAP_SIZE; X += MAP_RESOLUTION )
		for ( Y = 0; Y < HEIGHTMAP_SIZE; Y += MAP_RESOLUTION )
		{
			// Get the (X, Y, Z) value for the bottom left vertex		
			x = X;							
			y = GetPixelHeight(pHeightMap, X, Y );	
			z = Y;							

			// Set the color value of the current vertice
			SetVertexColor(pHeightMap, x, z);
			
			profile_glVertex3i(x, y, z);			// Send this vertex to OpenGL to be rendered 
											// (integer points are faster)

			// Get the (X, Y, Z) value for the top left vertex		
			x = X;										
			y = GetPixelHeight(pHeightMap, X, Y + MAP_RESOLUTION );  
			z = Y + MAP_RESOLUTION ;							

			// Set the color value of the current vertex
			SetVertexColor(pHeightMap, x, z);

			profile_glVertex3i(x, y, z);			// Send this vertex to OpenGL to be rendered

			// Get the (X, Y, Z) value for the top right vertex		
			x = X + MAP_RESOLUTION; 
			y = GetPixelHeight(pHeightMap, X + MAP_RESOLUTION, Y + MAP_RESOLUTION ); 
			z = Y + MAP_RESOLUTION ;

			// Set the color value of the current vertex
			SetVertexColor(pHeightMap, x, z);

			profile_glVertex3i(x, y, z);			// Send this vertex to OpenGL to be rendered

			// Get the (X, Y, Z) value for the bottom right vertex		
			x = X + MAP_RESOLUTION; 
			y = GetPixelHeight(pHeightMap, X + MAP_RESOLUTION, Y ); 
			z = Y;

			// Set the color value of the current vertice
			SetVertexColor(pHeightMap, x, z);

			profile_glVertex3i(x, y, z);			// Send this vertex to OpenGL to be rendered
		}
		glEnd();

		// Reset the color
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		GL_ErrorReport();
}

//
//	NAME: CompileHeightMapTextured
//
// ZWECK: Kompiliert eine Hight-Map und mappt eine Textur dar�ber
//
// KOMMENTAR: Gibt einen eindeutigen Bezeichner f�r die Liste zur�ck
//			  �ber diesen kann die Liste dann angesprochen werden
//
GLuint CompileHeightMapTextured(BYTE *pHeightMap, GLuint textureID) 
{
	
	// glGenLists(N) erzeugt N viele Listeneintr�ge und gibt einen Bezeichner
	// (als GLuint) �ber den die Liste angesprochen werden kann, zur�ck.
	// Da hier nur ein Objekt gezeichnet wird, ben�tigen wir 1 Eintrag.
	GLuint listname = glGenLists(1);

	// Displayliste generieren
	glNewList(listname, GL_COMPILE);	
		glPushMatrix();
			// Skalieren der Heightmap
			glScalef(20.0f, 20.0f, 20.0f);
			// Zentrieren der Landschaft
			glTranslatef( -HEIGHTMAP_SIZE / 2, -256.0f, -HEIGHTMAP_SIZE / 2);
			DrawHeightMapTextured(pHeightMap, textureID);
		glPopMatrix();
	// Ende der Kompilierten Liste
	glEndList();
	// Testen, ob die Display-Liste erfolgreich angelegt wurd
	assert(glIsList(listname) == GL_TRUE && "CompileHeightMapTextured: display list does not exist.");

	GL_ErrorReport();

	return listname;
}

//
//	NAME: DrawHeightMapTextured
//
// ZWECK: Zeichnet eine Height-Map und mappt eine Textur dar�ber
//
void DrawHeightMapTextured(BYTE *pHeightMap,					 // RAW-Bitmap Array
						      GLuint textureID)					 // TexturID f�r die Map
{
	int X = 0, Y = 0;						// Create some variables to walk the array with.
	int x, y, z;							// Create some variables for readability
	GLboolean bSwitchSides = GL_FALSE;

	// Make sure our height data is valid
	assert(pHeightMap || "DrawHeightMapTextured: invalid byte-array.");

	// The difference from the way we render the terrain from
	// the original way we started with, is that we don't use GL_QUADS
	// anymore, we now use a GL_TRIANGLE_STIP.  This means that we don't
	// need to pass in the same vertex more than once.  Each 2 vertices
	// are connected to the next 2.  Since we want to do this in one strip,
	// we are going to need to reverse the order every other column.  It's
	// like mowing the lawn.  Go to the end and turn around and come back
	// the way you came.  If you don't do it this way, you will get polygons
	// stretching across the whole terrain.  We could just do a new glBegin()
	// and glEnd() for every column, but I believe this way is faster.  
	// Not that that really matters though, because rendering a terrain
	// with glVertex*() calls in incredibly slow.  We will most likely want
	// to eventually switch this to vertex arrays.

	glEnable(GL_TEXTURE_2D);
	// Bind the terrain texture to our terrain
	glBindTexture(GL_TEXTURE_2D, textureID);

	// We want to render triangle strips
	glBegin( GL_TRIANGLE_STRIP );			

	// Go through all of the rows of the height map
	for ( X = 0; X <= HEIGHTMAP_SIZE; X += MAP_RESOLUTION )
	{
		// Chechk if we need to render the opposite way for this column
		if(bSwitchSides)
		{	
			// Render a column of the terrain, for this current X.
			// We start at HEIGHTMAP_SIZE and render down to 0.
			for ( Y = HEIGHTMAP_SIZE; Y >= 0; Y -= MAP_RESOLUTION )
			{
				// Get the (X, Y, Z) value for the bottom left vertex		
				x = X;							
				y = GetPixelHeight(pHeightMap, X, Y );	
				z = Y;							

				// Set the current texture coordinate and render the vertex
				// Normalisierte Texturkoordinaten auf das aktuell gezeichnete Vertex
				// umrechnen, da die gesamte Textur gemappt wird und nicht in St�cken.
				glTexCoord2f(   (float)x / (float)HEIGHTMAP_SIZE,	
					- (float)z / (float)HEIGHTMAP_SIZE	);
				profile_glVertex3i(x, y, z);		

				// Get the (X, Y, Z) value for the bottom right vertex		
				x = X + MAP_RESOLUTION; 
				y = GetPixelHeight(pHeightMap, X + MAP_RESOLUTION, Y ); 
				z = Y;

				// Set the current texture coordinate and render the vertex
				glTexCoord2f(   (float)x / (float)HEIGHTMAP_SIZE,	
					- (float)z / (float)HEIGHTMAP_SIZE	);
				profile_glVertex3i(x, y, z);			
			}
		}
		else
		{	
			// Render a column of the terrain, for this current X.
			// We start at 0 and render down up to HEIGHTMAP_SIZE.
			for ( Y = 0; Y <= HEIGHTMAP_SIZE; Y += MAP_RESOLUTION )
			{
				// Get the (X, Y, Z) value for the bottom right vertex		
				x = X + MAP_RESOLUTION; 
				y = GetPixelHeight(pHeightMap, X + MAP_RESOLUTION, Y ); 
				z = Y;

				// Set the current texture coordinate and render the vertex
				glTexCoord2f(   (float)x / (float)HEIGHTMAP_SIZE,	
					- (float)z / (float)HEIGHTMAP_SIZE	);
				profile_glVertex3i(x, y, z);

				// Get the (X, Y, Z) value for the bottom left vertex		
				x = X;							
				y = GetPixelHeight(pHeightMap, X, Y );	
				z = Y;							

				// Set the current texture coordinate and render the vertex
				glTexCoord2f(   (float)x / (float)HEIGHTMAP_SIZE,	
					- (float)z / (float)HEIGHTMAP_SIZE	);
				profile_glVertex3i(x, y, z);		
			}
		}

		// Switch the direction the column renders to allow the fluid tri strips
		bSwitchSides = !bSwitchSides;
	}

	// Stop rendering triangle strips
	glEnd();

	glDisable(GL_TEXTURE_2D);

	GL_ErrorReport();
}

// Quellcodeglobale Variablen f�r das Vertex-Array der Height-Map
static texcoords *texcoords_array = NULL;
static vertexcoords *vertex_array = NULL;
static normalcoords *normal_array = NULL;

//
// Heightmap als Vertex-Array initialisieren
//
GLvoid initHeightmapVA(BYTE *pHeightMap)
{

	int X = 0, Y = 0;						// Create some variables to walk the array with.
	int x, y, z;							// Create some variables for readability
	GLboolean bSwitchSides = GL_FALSE;
	
	// Make sure our height data is valid
	assert(pHeightMap || "DrawHeightMapTextured: invalid byte-array.");
	
	// Speicher f�r das Objekt allokieren
	texcoords_array = (texcoords *)malloc( HEIGHTMAP_SIZE / MAP_RESOLUTION * HEIGHTMAP_SIZE /MAP_RESOLUTION * sizeof(texcoords));
	vertex_array = (vertexcoords *)malloc( HEIGHTMAP_SIZE / MAP_RESOLUTION * HEIGHTMAP_SIZE /MAP_RESOLUTION * sizeof(vertexcoords));
	
	assert( (texcoords_array != NULL || vertex_array != NULL) 
			&& "initHeightmapVA: memory allocation failed.");

	// Debug-Meldungen
	devconout("Building vertex array for heightmap ...\n");
	devconout(" t = %i Kb", (HEIGHTMAP_SIZE / MAP_RESOLUTION * HEIGHTMAP_SIZE /MAP_RESOLUTION * sizeof(texcoords)) / 1024);
	devconout(", v = %i Kb\n", (HEIGHTMAP_SIZE / MAP_RESOLUTION * HEIGHTMAP_SIZE /MAP_RESOLUTION * sizeof(vertexcoords)) / 1024);

	// Schrittweite = Gr��e der Vertex-Arrays
	int stepwidth = (HEIGHTMAP_SIZE / MAP_RESOLUTION);

	// Indizes f�r die vertex-arrays
	// Da diese eine andere Gr��e haben als die Heightmap 
	// m�ssen eigene Z�hler daf�r definiert werden
	int array_x, array_y;
	
	// Go through all of the rows of the height map
	for ( X = 0, array_x = 0;
		  X <= HEIGHTMAP_SIZE;
		  X += MAP_RESOLUTION, array_x++ )
	{
		// Pr�fen auf array-out of bounds
		assert(array_x <=stepwidth && "initHeightmapVA: x-index out of range." );
		
		// Chechk if we need to render the opposite way for this column
		// Rasenm�herprinzip
		if(bSwitchSides)
		{	
			// Render a column of the terrain, for this current X.
			// We start at HEIGHTMAP_SIZE and render down to 0.
			for ( Y = HEIGHTMAP_SIZE, array_y = stepwidth;
				  Y >= 0;
				  Y -= MAP_RESOLUTION, array_y-- )
			{
				// bounds-check
				assert(array_y >= 0 && "initHeightmapVA : y-index smaller than zero.");
				
				// Get the (X, Y, Z) value for the bottom left vertex		
				x = X;							
				y = GetPixelHeight(pHeightMap, X, Y );	
				z = Y;							

				// Set the current texture coordinate and render the vertex
				//glTexCoord2f(   (float)x / (float)HEIGHTMAP_SIZE,	
				//	- (float)z / (float)HEIGHTMAP_SIZE	);
				// treat the single array like a 2D array.
				// We can use the equation: index = (x + (y * arrayWidth) ).
				int idx = array_x+(array_y* stepwidth);
				
				texcoords_array[idx].s = (float)x / (float)HEIGHTMAP_SIZE;
				texcoords_array[idx].t = - (float)z / (float)HEIGHTMAP_SIZE;
				
				//profile_glVertex3i(x, y, z);		
				vertex_array[idx].x = (float)x;
				vertex_array[idx].y = (float)y;
				vertex_array[idx].z = (float)z;

				// Get the (X, Y, Z) value for the bottom right vertex		
				x = X + MAP_RESOLUTION; 
				y = GetPixelHeight(pHeightMap, X + MAP_RESOLUTION, Y ); 
				z = Y;

				// Set the current texture coordinate and render the vertex
				//glTexCoord2f(   (float)x / (float)HEIGHTMAP_SIZE,	
				//	- (float)z / (float)HEIGHTMAP_SIZE	);
				texcoords_array[idx].s = (float)x / (float)HEIGHTMAP_SIZE;
				texcoords_array[idx].t = - (float)z / (float)HEIGHTMAP_SIZE;

				//profile_glVertex3i(x, y, z);		
				vertex_array[idx].x = (float)x;
				vertex_array[idx].y = (float)y;
				vertex_array[idx].z = (float)z;			
			}
		}
		else
		{	
			// Render a column of the terrain, for this current X.
			// We start at 0 and render down up to HEIGHTMAP_SIZE.
			for ( Y = 0, array_y = 0;
				  Y <= HEIGHTMAP_SIZE;
				  Y += MAP_RESOLUTION, array_y++ )
			{
				assert(array_y <= stepwidth && "initHeightmapVA: y-index out of range.");

				// Get the (X, Y, Z) value for the bottom right vertex		
				x = X + MAP_RESOLUTION; 
				y = GetPixelHeight(pHeightMap, X + MAP_RESOLUTION, Y ); 
				z = Y;
				
				int idx = array_x+(array_y* stepwidth);

				// Set the current texture coordinate and render the vertex
				//glTexCoord2f(   (float)x / (float)HEIGHTMAP_SIZE,	
				//	- (float)z / (float)HEIGHTMAP_SIZE	);
				texcoords_array[idx].s = (float)x / (float)HEIGHTMAP_SIZE;
				texcoords_array[idx].t = - (float)z / (float)HEIGHTMAP_SIZE;

				//profile_glVertex3i(x, y, z);		
				vertex_array[idx].x = (float)x;
				vertex_array[idx].y = (float)y;
				vertex_array[idx].z = (float)z;

				// Get the (X, Y, Z) value for the bottom left vertex		
				x = X;							
				y = GetPixelHeight(pHeightMap, X, Y );	
				z = Y;							

				// Set the current texture coordinate and render the vertex
				//glTexCoord2f(   (float)x / (float)HEIGHTMAP_SIZE,	
				//	- (float)z / (float)HEIGHTMAP_SIZE	);
				texcoords_array[idx].s = (float)x / (float)HEIGHTMAP_SIZE;
				texcoords_array[idx].t = - (float)z / (float)HEIGHTMAP_SIZE;

				//profile_glVertex3i(x, y, z);		
				vertex_array[idx].x = (float)x;
				vertex_array[idx].y = (float)y;
				vertex_array[idx].z = (float)z;		
			}
		}

		// Switch the direction the column renders to allow the fluid tri strips
		bSwitchSides = !bSwitchSides;
	}

	GL_ErrorReport();
}

//
// Heightmap als vertex array rendern
//
void renderHeightmapVA(int textureid)
{
	// Pr�fen der Arrays auf G�ltigkeit
	assert((vertex_array || normal_array || texcoords_array) && 
		   "renderHeighmapVA: vertex arrays uninitialized.");

	glPushMatrix();
		
		//////////////////////////////////////////////////////////////////////////
		// Draw the entire object.
		//////////////////////////////////////////////////////////////////////////
		
		// Vertex-Array vorbereiten
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		//glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);

		// Load the data to each pointer type we need.
		// Das wird deshalb getan, da unterschiedliche Vertex-Arrays existieren k�nnen
		// glDrawArrays sorgt allerdings daf�r, dass alle per gl...Pointer referenzierten
		// Arrays gerendert werden. Deshalb werden hier explizit die Pointer f�r dieses
		// Objekt angegeben
		glVertexPointer(3, GL_FLOAT, 0, vertex_array);
		//glNormalPointer(GL_FLOAT, 0, normal_array);
		glTexCoordPointer(2, GL_FLOAT, 0, texcoords_array);

		// Textur binden
		glBindTexture(GL_TEXTURE_2D, textureid);

		// Die im Vertex-Array enthaltenen Verticies werden dem Profiler
		// gemeldet. Das deshalb, weil Vertex-Arrays nicht per glVertex3f() gerendert
		// werden und somit durch profile_glVertex3f() ersetzt werden k�nnten
		//profile_addVertexCount(num_verticies);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 256*256); // TODO: Wert fixen

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		//glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
	
	GL_ErrorReport();
}

//
// Speicher freigeben
//
void FreeHeightMapVA()
{
	free(texcoords_array);
	free(vertex_array);
}
//
//	NAME: GetPixelHeight
//
// ZWECK: Ermittelt die H�he des zu zeichnenden Pixels auf der Map
//
int GetPixelHeight(BYTE *pHeightMap, int X, int Y)
{
	// This is used to index into our height map array.
	// When ever we are dealing with arrays, we want to make sure
	// that we don't go outside of them, so we make sure that doesn't
	// happen with a %.  This way x and y will cap out at (MAX_SIZE - 1)


	// x ist immer = X solange X < HEIGHTMAP_SIZE
	int x = X % HEIGHTMAP_SIZE;					// Error check our x value
	int y = Y % HEIGHTMAP_SIZE;					// Error check our y value

	// Make sure our data is valid
	assert(pHeightMap && "GetPixelHeight: Invalid pointer to heightmap data");

	// Below, we need to treat the single array like a 2D array.
	// We can use the equation: index = (x + (y * arrayWidth) ).
	// This is assuming we are using this assumption array[x][y]
	// otherwise it's the opposite.  Now that we have the correct index,
	// we will return the height in that index.

	return pHeightMap[x + (y * HEIGHTMAP_SIZE)];	// Index into our height array and return the height
}


//
// NAME SetVertexColor
//
// ZWECK: Ermittelt den Farbwert des Vertex auf der H�henkarte um Beleuchtung
//		  zu simulieren
//
void SetVertexColor(BYTE *pHeightMap, int x, int y)
{
	// Vertex-Color
	float fColor = 0;

	if(!pHeightMap) return;					// Make sure our height data is valid

	// Here we set the color for a vertex based on the height index.
	// To make it darker, I start with -0.15f.  We also get a ratio
	// of the color from 0 to 1.0 by dividing the height by 256.0f;
	fColor = -0.15f + (GetPixelHeight(pHeightMap, x, y ) / 256.0f);

	// Assign this green shade to the current vertex
	glColor3f(0, fColor, 0 );
}

//
// Diese Funktion rendert die B�ume auf der Heightmap anhand einer RAW-Treemap Datei
// Das Prinzip ist das gleiche wie beim Heightmapping. Schwarz bedeutet keine B�ume und
// anhand des Grautons kann entschieden werden welche B�ume wo platziert werden.
// Au�erdem wird jeder Baum als Billboard gerendert.
//
// Die Tree-Map kann nicht per Display-Liste optimiert werden, da die B�ume als
// Billboards dynamisch dargestellt werden (Displaylisten eignen sich nur f�r
// statische Objekte).
// 
void RenderTreeMap(BYTE *treemap, BYTE *heightmap,	// Referenz auf die Treemap- und Heightmapbytearrays
				   GLuint treevariant1,				// Baumtexturen
				   GLuint treevariant2,
				   GLuint treevariant3,
				   GLuint treevariant4,
				   clipping_plane_t *planes)		// Die Clipping-Panes des View-Frustums
{
	int pixelvalue = 0;
	int pixelheight = 0;
	int treecount = 0;

	// Daten validieren
	assert( (treemap != NULL|| heightmap != NULL ) && "RenderTreeMap: invalid height- or treemap.");
	assert( planes != NULL && "RenderTreeMap: clipping pane array invalid.");

	// Alpha Blending aktivieren, damit die Baumtexturen an den richtigen
	
	// Stellen transparent erscheinen
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_TEXTURE_2D);
	//glDisable(GL_CULL_FACE);
	

	//////////////////////////////////////////////////////////////////////////
	// Billboarding
	//////////////////////////////////////////////////////////////////////////
	
	// F�r das Billboarding notwendige Variablen
	vector3f_t up;// = NewEmptyVector();			// Zeigt nach oben im model-space
	vector3f_t right;// = NewEmptyVector();		// Zeigt nach rechts im model-space
	vector3f_t center;// = NewEmptyVector();		// Zentrum des Billboards
	vector3f_t vertex;// = NewEmptyVector();		// zu rendernder vertex
	
	// Ausrichtung des UP und RIGHT Vektors aus der transponierten Modelviewmatrix holen
	BillboardGetUpRightVector(&up, &right);
	
	// Berechnen der Skalarprodukte up*size und right*size aus Effizienzgr�nden
	// TREE_SIZE gibt die gr��e des Baums an
	vector3f_t up_size = VectScalarMult(up, TREE_SIZE);
	vector3f_t right_size = VectScalarMult(right, TREE_SIZE);

	
	//glPushMatrix();
	
	// �ber die treemap laufen in definierter Schrittweite
	for ( int x = 0; x <= HEIGHTMAP_SIZE; x += TREEMAP_RESOLUTION )
	{
		for (int y = 0; y <= HEIGHTMAP_SIZE; y += TREEMAP_RESOLUTION)
		{
			// Farbwert gibt an ob Baum zu setzen ist
			pixelvalue = GetPixelHeight(treemap, x, y);
			// H�he der Stelle an der der Baum gesetzt wird
			pixelheight = GetPixelHeight(heightmap, x, y);	
			
			if(pixelvalue > 50)
			{
				// Ausw�hlen der Baumtextur, je nach Farbwert			
				if(pixelvalue >= 51 && pixelvalue <= 80)
					glBindTexture(GL_TEXTURE_2D, treevariant3);
				else if (pixelvalue > 80 && pixelvalue <= 120)
					glBindTexture(GL_TEXTURE_2D, treevariant1);
				else if (pixelvalue > 120 && pixelvalue <= 150)
					glBindTexture(GL_TEXTURE_2D, treevariant2);
				else if ( pixelvalue < 180)
					glBindTexture(GL_TEXTURE_2D, treevariant4);
				
				// Zentrum des Billboards f�r den Baum bestimmen (unten/mitte)
				center.x = (float)x;
				center.y = (float)pixelheight;
				center.z = (float)y;
				
				// Frustum-Culling f�r den Center-Point bestimmen
				// TODO: B�ume am unteren Bildrand "poppen" unsch�n auf
				bool inside = isPointVisibleOnPlane(planes, NewVector((float)x,(float)pixelheight,(float)y)/*center*/);
				
				// Wenn der Punkt sichtbar ist, dann den Baum rendern
				if(inside)
				{
					// Schnelles Billboarding nutzen
					//a = center - right * (size * 0.5); // unten links
					//b = center + right * size * 0.5;	 // unten rechts
					//c = center + right * size * 0.5 + up * size;  // oben rechts
					//d = center - right * size * 0.5 + up * size;	// oben links

					glBegin(GL_QUADS);
						// Berechnen und rendern der Vertex-Koordinaten
						vertex = SubtractVector(center, right_size);
						glTexCoord2f(0.0f, 0.0f); //glVertex3f(vertex.x, vertex.y, vertex.z);  
						profile_glVertex3f(vertex.x, vertex.y, vertex.z);
						vertex = AddVector(center, right_size);
						glTexCoord2f(1.0f, 0.0f); //glVertex3f(vertex.x, vertex.y, vertex.z);  
						profile_glVertex3f(vertex.x, vertex.y, vertex.z);
						vertex = AddVector(AddVector(center, right_size),up_size);
						glTexCoord2f(1.0f, 1.0f); //glVertex3f(vertex.x, vertex.y+3, vertex.z); 
						profile_glVertex3f(vertex.x, (vertex.y+3), vertex.z);
						vertex = AddVector(SubtractVector(center, right_size), up_size);
						glTexCoord2f(0.0f, 1.0f); //glVertex3f(vertex.x, vertex.y+3, vertex.z); 
						profile_glVertex3f(vertex.x, (vertex.y+3), vertex.z);
						
						/*glTexCoord2f(0.0f, 0.0f); profile_glVertex3f(x, pixelheight, y);  
						glTexCoord2f(1.0f, 0.0f); profile_glVertex3f(x+TREE_SIZE, pixelheight, y );  
						glTexCoord2f(1.0f, 1.0f); profile_glVertex3f(x+TREE_SIZE, pixelheight+TREE_SIZE, y);  
						glTexCoord2f(0.0f, 1.0f); profile_glVertex3f(x, pixelheight+TREE_SIZE,y);*/
					glEnd();					
					/*glPushMatrix();				
					glRotatef(90.0f, 0, 1.0f, 0);
					glBegin(GL_QUADS);
						glTexCoord2f(0.0f, 0.0f); profile_glVertex3f(x, pixelheight, y);  
						glTexCoord2f(1.0f, 0.0f); profile_glVertex3f(x+TREE_SIZE, pixelheight, y );  
						glTexCoord2f(1.0f, 1.0f); profile_glVertex3f(x+TREE_SIZE, pixelheight+TREE_SIZE, y);  
						glTexCoord2f(0.0f, 1.0f); profile_glVertex3f(x, pixelheight+TREE_SIZE,y);
					glEnd();
					glPopMatrix();
					*/
				} //if
				
				// Gesamtzahl der B�ume z�hlen (nicht der effektiv gerenderten)
				treecount++;

			} //if (pixelvalue > 50)

		} // innere for

	}// �u�ere for

	//glPopMatrix();

	static bool output = false;
	if(!output)
	{
		devconout("Treemap: rendering %d trees\n", treecount);
		output = true;
	}

	// Den Zustandsautomaten wieder zur�cksetzen
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	//glEnable(GL_CULL_FACE);

	GL_ErrorReport();
}