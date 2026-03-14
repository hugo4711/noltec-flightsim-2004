//////////////////////////////////////////////////////////////////////////
// Height-Map
//

#include "../frustum.h"

#ifndef _GEOMAP_H
#define _GEOMAP_H

// TODO: Diese Konstanten sollten aus einer externen Konfigurationsdatei gelesen werden

#define TREE_SIZE			0.6f		// Größe eines Baums
#define TREEMAP_RESOLUTION	25			// Auflösung beim Abtasten der Treemap


#define HEIGHTMAP_SIZE		4096  // Quadratische Height-Map Größe
#define MAP_RESOLUTION		16	  // Größe der einzelnen zu zeichnenden Planquadrate der Map (quadratisch)
								  // Eine kleinere Größe erhöht die Darstellungsqualität und Rechenleistung!

// Strukturen für das Vertex-Array
#include "vertex_array.h"

// Rendert die zuvor eingelesenen RAW-Daten zu einer Height-Map
void DrawHeightMapShaded(BYTE *pHeightMap);
// Rendert eine Height-Map und mappt eine Textur darüber
GLuint CompileHeightMapTextured(BYTE *pHeightMap, GLuint textureID);
// Zeichnet eine Hight-Map und mappt eine Textur darüber
void DrawHeightMapTextured(BYTE *pHeightMap, GLuint textureID);
// Gibt den Höhenwert eines Pixels (X,Y) in der Height-Map zurück
int GetPixelHeight(BYTE *pHeightMap, int X, int Y);
// Ermittelt die Farbe des Vertex anhand des Höhenwertes in der Map - simuliert Beleuchtung
void SetVertexColor(BYTE *pHeightMap, int x, int y);
// Heightmap als Vertex-Array initialisieren
GLvoid initHeightmapVA(BYTE *pHeightMap);
// Heightmap als Vertex-Array rendern
GLvoid renderHeightmapVA(int textureid);
// Speicher freigeben
void FreeHeightMapVA();

// Rendern der Trees basierend auf der Treemap
void RenderTreeMap(BYTE *treemap, BYTE *heightmap,	// Referenz auf die Treemap- und Heightmapbytearrays
				   GLuint treevariant1,				// Baumtexturen
				   GLuint treevariant2,
				   GLuint treevariant3,
				   GLuint treevariant4,
				   clipping_plane_t *planes);

#endif /* _HEIGHTMAP_H */