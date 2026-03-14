// Initialisierungsfunktionen und Verwaltungsfunktionen f�r den Rendercontext
#include "stdafx.h"
#include <strsafe.h>
#include "glscene.h"

#include "camera.h"				// Kamerasteuerung
#include "objects/cube.h"		// W�rfelobjekt
#include "objects/geomap.h"		// Height-Map
#include "skybox.h"				// Skybox
#include "math/vectormath.h"	// Vektormathematik
#include "displaylist.h"		// Konstanten f�r die Display-Listen
#include "2Dfonts.h"			// 2D Schrift-Listenerstellung f�r das Rendering
#include "loader.h"				// Texturloader 
#include "particlesystem.h"		// Partikelsystem
#include "physics/airplane.h"   // Flugphysik
#include "sound/snd_general.h"	// Generelle Definitionen f�r den Sound
#include "billboarding.h"		// Billboarding
#include "objects/f18hornet.h"  // Das Flugzeugobjekt
#include "3ds/3dsmodells.h"		// 3DS Modellkonstanten
#include "frustum.h"			// F�r die Bestimmung des Frustums und das dazugeh�rige Culling


//////////////////////////////////////////////////////////////////////////
// Array mit den Textur-Informationen. Die Indizes m�ssen den definierten Konstanten
// in der Header-Datei entsprechen (Definitionen in texture.h)
//////////////////////////////////////////////////////////////////////////

static ldr_textureinf_t textureInfo[] = 
{
	// FORMAT:
	// Texturname,		|	Format  |ID | Mipmapping?
	//-----------------------------------------------
	CRATE_TEXTURE_NAME,		FMT_BMP, 0,		GL_TRUE,	// Erster Eintrag im Array-Of-Structs (idx = 0)
	// Texturen f�r die Height-Map
	HEIGHT_MAP_NAME,		FMT_RAW, 0,		GL_FALSE,  // Bei RAW ist Mipmaping egal weil andere Laderoutine
	GROUND_TEXTURE_NAME,	FMT_TGA, 0,		GL_TRUE,
	// Skybox Texturen
	SKYAHEAD_TEXTURE_NAME,	FMT_BMP, 0,		GL_FALSE,
	SKYBACK_TEXTURE_NAME,	FMT_BMP, 0,		GL_FALSE,
	SKYRIGHT_TEXTURE_NAME,  FMT_BMP, 0,		GL_FALSE,	
	SKYLEFT_TEXTURE_NAME,	FMT_BMP, 0,		GL_FALSE,
	SKYUP_TEXTURE_NAME,		FMT_BMP, 0,		GL_FALSE,
	SKYDOWN_TEXTURE_NAME,	FMT_BMP, 0,		GL_FALSE,
	// GL-Logo
	GLLOGO_TEXTURE_NAME,	FMT_TGA, 0,		GL_FALSE,
	// Cockpit der F18
	F18COCKPIT_TEXTURE_NAME,FMT_TGA, 0,		GL_FALSE,
	// Treemap
	TREE_MAP_NAME,			FMT_RAW, 0,		GL_FALSE,
	// Trees
	TREE1_TEXTURE_NAME,		FMT_TGA, 0,		GL_FALSE,
	TREE2_TEXTURE_NAME,		FMT_TGA, 0,		GL_FALSE,
	TREE3_TEXTURE_NAME,		FMT_TGA, 0,		GL_FALSE,
	TREE4_TEXTURE_NAME,		FMT_TGA, 0,		GL_FALSE,
	MAINSCREEN_NAME,		FMT_TGA, 0,		GL_FALSE,
    LENSF1_NAME,			FMT_TGA, 0,		GL_FALSE,
	LENSF2_NAME,			FMT_TGA, 0,		GL_FALSE,
	LENSF3_NAME,			FMT_TGA, 0,		GL_FALSE,
	LENSF4_NAME,			FMT_TGA, 0,		GL_FALSE,
	ROCKETTRAIL_NAME,		FMT_TGA, 0,		GL_FALSE
	// BEMERKUNG: ID muss 0 sein! Wird beim Laden ermittelt!
};

//////////////////////////////////////////////////////////////////////////
// Array f�r die 3DS Modelle
//////////////////////////////////////////////////////////////////////////

/*static object3ds_t object3dsInfo[] =
{
	// FORMAT:
	// Objektname,		|  Objektzgr | ID
	//--------------------------------------------------------
	C5CARGO_OBJECT3DS_NAME,	NULL,		C5CARGO_OBJECT3DS_IDX,
	F18_OBJECT3DS_NAME,		NULL,		F18_OBJECT3DS_IDX,
	MISSILE_OBJECT3DS_NAME, NULL,		MISSILE_OBJECT3DS_IDX
	// BEMERKUNG: ID wird nicht ben�tigt,
	// Objekt-Zeiger ist mit NULL zu initialisieren
};*/

//////////////////////////////////////////////////////////////////////////
// Sound
//////////////////////////////////////////////////////////////////////////

static sndDX9_soundobject_t **sndobjectDX9; // DirectX9 DirectSound

//////////////////////////////////////////////////////////////////////////
// Objekte und Objektlisten
//////////////////////////////////////////////////////////////////////////

// Heightmap als bin�re Daten
static BYTE *heightmap;
// Treemap als bin�re Daten
static BYTE *treemap;

// Datenstruktur zur W�rfel-Objektverwaltung
static cube_t *cubeInformation;
// Verwaltung der Display-Listen der Objekte. Enth�lt jeweils den
// Base-Pointer f�r die Addressierung der Liste
static GLuint *ObjectListID;

//////////////////////////////////////////////////////////////////////////
// F�r die Clipping-Plane Berechnung
//////////////////////////////////////////////////////////////////////////

//View-Frustum im Model-Space
static clipping_plane_t mvspace_planes[6];

//////////////////////////////////////////////////////////////////////////
// Partikelsystem
//////////////////////////////////////////////////////////////////////////
static particlepool_t pool;		// Der Pool
static emitter_t em;			// Der Emitter


// TODO: Framerateberechnung!!
static int time_progstart;
float	  frame_interval = 0.0f;  // Zeit um einen Frame zu zeichnen


// Bildschirmaufl�sung (s. ResizeGLScene), ben�tigt f�r die Camerabewegung
// und Viewport-Ausdehnung (f�r Schriftpositionierung)
static int viewport_width  = 0;
static int viewport_height = 0;

// Statusinformationen
// gibt an, ob ein Drahtgittermodell gerendert werden soll
static GLboolean render_wireframe = GL_FALSE;	
static GLboolean pause = GL_TRUE;

// Spielelogik
// TODO: Raketenlogik komplett ausgliedern und ein Rocket-Struct anlegen!
static GLboolean fireMissile = GL_FALSE;		// Raketenabschuss?
static float posz = 0;							// Raketen z-Pos
static float phi = 0;							// Raketenrotation

// Starrer K�rper - Physikalisches Flugverhalten
static rigidbody_t f18hornet;

// Funktion: InitGL
//
// ZWECK: Initialisiern globaler Einstellungen f�r das Rendering
//
// KOMMENTAR: Hier geh�ren alle globalen Einstellungen f�r das GL Rendering hinein.
//			  Zum Beispiel k�nnen hier auch die Texturen geladen werden		
//			  Ruft ResizeGLScene() auf, um den Renderbereich erstmalig einzustellen
//	
// R�CKGABEWERT: GL_FALSE - Fehler beim Initialisieren, ansonsten GL_TRUE
//
GLboolean InitGL(HDC hDC, GLsizei width, GLsizei height) {
	// Fehlermeldungen, die beim Laden auftreten
	char error_message[256] = { "" };
	// Laufz�hler f�r die Texturladeschleife
	int counter;

	devconout("initializing gl:\n");
	
	//////////////////////////////////////////////////////////////////////////
	// Kamera
	//////////////////////////////////////////////////////////////////////////
	

	// Echte Bildschirmaufl�sung ermitteln
	devconout("getting screen coords: ");

	int screen_height   = GetSystemMetrics(SM_CXSCREEN);
	int screen_width    = GetSystemMetrics(SM_CYSCREEN);

	// Pr�fen, ob erfolgreich
	assert(screen_width != NULL && screen_height != NULL && 
			"InitGL: GetSystemMetrics failed.");

	devconout("%d x %d\n", screen_height, screen_width);

	// Kamera auf die Bildschirmaufl�sung vorbereiten (wird f�r die
	// Maussteuerung ben�tigt
	SetScreenDimensions(screen_height, screen_width);
	// Kameraposition initialisieren
	InitCamera(NewVector(0,0,40.0f), NewVector(0,0,0), NewVector(0.0,1.0f,0) );

	//////////////////////////////////////////////////////////////////////////
	// GL Stati einstellen
	//////////////////////////////////////////////////////////////////////////
	
	glShadeModel(GL_FLAT);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_LINEAR);

	//////////////////////////////////////////////////////////////////////////
	// Nebel
	//////////////////////////////////////////////////////////////////////////
	
	glEnable(GL_FOG);
	glFogf(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.00001f);
	float fogcolor[] = {0.8f,0.8f,0.8f};
	glFogfv(GL_FOG_COLOR, fogcolor);
	glHint(GL_FOG_HINT, GL_FASTEST); // per vertex-fogging

	//////////////////////////////////////////////////////////////////////////
	// Texturen und Height-Map laden
	//
	//////////////////////////////////////////////////////////////////////////
	
	// Debug Assertion, wenn die Texturanzahl nicht mit den zu ladenden �bereinstimmt
	// dann liegt ein Definitionsfehler in der texture.h vor!
	assert(sizeof(textureInfo) == LDR_TEXTURE_CNT * sizeof(ldr_textureinf_t) &&
		   "InitGL: texture-struct size not equal to texture count.");

	devconout("loading textures :\n");

	// S�mtliche Texturen laden
	for(counter = 0; counter < LDR_TEXTURE_CNT; counter++) {
		devconout("loading %s ...\n", textureInfo[counter].textureName);
		// R�ckgabewert pr�fen
		switch( LoadTexture( textureInfo[counter].textureName,
							 textureInfo[counter].textureType,
							 &textureInfo[counter].textureID,
							 textureInfo[counter].mipmapped) )
		{
			case  0:
				break;
			case -1:
				// Nicht gen�gend Speicher zum Laden der Texturen
				StringCbPrintf(error_message, sizeof(error_message)-1,
							   "%s : not enough memory available.",
							   textureInfo[counter].textureName);
				//MessageBox(NULL, error_message,
				//		   "Error", MB_OK | MB_ICONEXCLAMATION);
				devconout("ERROR: %s\n", error_message);
				break;
			case -2:
				// Datei nicht gefunden
				StringCbPrintf(error_message, sizeof(error_message)-1,
							   "%s :file not found.",
							   textureInfo[counter].textureName);
				//MessageBox(NULL, error_message,
				//		  "Error", MB_OK | MB_ICONEXCLAMATION);
				devconout("ERROR: %s\n", error_message);
				break;
			case -3:
				// Texturtyp nicht unterst�tzt
				StringCbPrintf(error_message, sizeof(error_message)-1,
							   "%s : texture type %d not supported.",
								textureInfo[counter].textureName,
								textureInfo[counter].textureType);
				//MessageBox(NULL, error_message,
				//	"Error", MB_OK | MB_ICONEXCLAMATION);
				devconout("ERROR: %s\n", error_message);
				break;
			case -4 :
				// -4 wird zur�ckgegeben wenn ein RAW-File geladen werden soll
				// das wird aber von diser Funktion nicht direkt unterst�tzt
				// Daf�r existiert eine extra Fkt. LoadRAWFile.

				// Ignorieren
				break;
			default:
				MessageBox(NULL, "unknown error occured while loading textures.",
						   "Error", MB_OK | MB_ICONEXCLAMATION);

		}
				
	}
	
	//////////////////////////////////////////////////////////////////////////
	//
	// Heightmap-Daten werden hier eingelesen
	//
	// RAW-Files k�nnen nicht mit LoadMipMapTexture geladen werden,
	// sondern nur direkt per LoadRAWFile()
	//////////////////////////////////////////////////////////////////////////

	// Quadratische Gr��e der Heightmap bestimmt die Array-Gr��e
	heightmap = (BYTE *)calloc(HEIGHTMAP_SIZE*HEIGHTMAP_SIZE, sizeof(BYTE));
	if(!heightmap)
		devconout("Not enough memory available.");
	else 
	{
		// Heightmap laden (auf quadratische Gr��e achten!)
		if( LoadRAWFile(textureInfo[HEIGHT_MAP_IDX].textureName,
						HEIGHTMAP_SIZE*HEIGHTMAP_SIZE, heightmap) != 0)
			devconout("heightmap could not be loaded.");
	}

	// TODO: Testen des Vertex-Arrays.
	//initHeightmapVA(heightmap);

	// Treemap laden (=Heightmapping!)
	treemap = (BYTE *)calloc(HEIGHTMAP_SIZE*HEIGHTMAP_SIZE, sizeof(BYTE));
	if(!treemap)
		devconout("Not enough memory available.");
	else 
	{
		// Heightmap laden (auf quadratische Gr��e achten!)
		if( LoadRAWFile(textureInfo[TREE_MAP_IDX].textureName,
						HEIGHTMAP_SIZE*HEIGHTMAP_SIZE, treemap) != 0)
			devconout("treemap could not be loaded.");
	}

	//////////////////////////////////////////////////////////////////////////
	// Objekte initialisieren
	//////////////////////////////////////////////////////////////////////////
	
	cubeInformation = Initcube();

	//////////////////////////////////////////////////////////////////////////
	// Auf Display-Listen basierte Darstellungen kompilieren
	//////////////////////////////////////////////////////////////////////////

	devconout("creating display lists...\n");

	// Listeneintr�ge genererieren, wie in displaylist.h definiert!
	// Die Base-Pointer der Displaylisten werden im dynamischen
	// Array ObjectListID gespeichert.
	ObjectListID = (GLuint *)calloc(OBJECT_LIST_COUNT, sizeof(GLuint));

	// TODO: Die Schrift-Displaylisten funktionieren nur in dieser Reihenfolge.
	//		 Werden diese zuletzt erzeugt, kann wglUseFontBitmap die erste
	//		 Schrift nicht erzeugen...
	//
	//		 Scheint ein Speicherallokationsproblem zu sein... Siehe Texture-Struct

	devconout("creating heightmap ...");
	// Height-Map
	ObjectListID[HEIGHTMAP_LIST_IDX] = CompileHeightMapTextured(heightmap,
									   textureInfo[GROUND_TEXTURE_IDX].textureID);
	devconout("heightmap list id = %i\n", ObjectListID[HEIGHTMAP_LIST_IDX]);
	// cube_t
	ObjectListID[CUBE_LIST_IDX] = CompileCube(cubeInformation,
									textureInfo[GLLOGO_TEXTURE_IDX].textureID);

	// Fonts erzeugen
	ObjectListID[VERDANAFONT_LIST_IDX]	= CreateGLFontList(hDC, "Verdana", FONT_HEIGHT);
	ObjectListID[ARIALFONT_LIST_IDX]	= CreateGLFontList(hDC, "Arial", FONT_HEIGHT);
	ObjectListID[TIMESFONT_LIST_IDX]	= CreateGLFontList(hDC, "Times", FONT_HEIGHT);

	//////////////////////////////////////////////////////////////////////////
	// 3DS Objekte laden
	//////////////////////////////////////////////////////////////////////////
	
	devconout("loading 3Ds objects : \n");

/*	// pr�fen, ob die Anzahl der Eintr�ge mit dem �bereinstimmt, was
	// in der 3DSmodells.h definiert wurde
	assert(sizeof(object3dsInfo) == OBJECT3DS_COUNT * sizeof(object3ds_t) &&
		   "InitGL: object3ds struct size not equal to object count.");

	for(counter = 0; counter < OBJECT3DS_COUNT; counter++)
	{
		devconout("%s ...\n", object3dsInfo[counter].name);	
		object3dsInfo[counter].object = new Object3ds(object3dsInfo[counter].name);
		// Auf g�ltiges 3DS Objekt pr�fen
		assert(object3dsInfo[counter].object != NULL &&
			   "InitGL: memory allocation for 3ds object failed.");
	}
*/
	//////////////////////////////////////////////////////////////////////////
	// NEUER LOADER...
	// frustrum.com Interface nutzen
	//////////////////////////////////////////////////////////////////////////
	
	devconout("%s ...\n", F18_OBJECT3DS_NAME);

	initF18Object();


	//////////////////////////////////////////////////////////////////////////
	// Partikelsystem einrichten
	//////////////////////////////////////////////////////////////////////////
	
	devconout("initializing particle system...\n");
	
	// Initialisieren des Partikelsystems
	if(!InitParticleSystem(textureInfo[GLLOGO_TEXTURE_IDX].textureID, &pool,&em))
        return GL_FALSE;

	devconout("particlepool contains %d particles\n", pool.particlein);
	devconout("emittername : %s\n", em.name);

	//////////////////////////////////////////////////////////////////////////
	// F�r die Framerate berechnung
	//////////////////////////////////////////////////////////////////////////
	
	time_progstart = (int)time(NULL);

	//////////////////////////////////////////////////////////////////////////
	// sin/cos Lookup-Table initialisieren
	//////////////////////////////////////////////////////////////////////////
	
	//build_lookuptables_sincos();
	
	//////////////////////////////////////////////////////////////////////////
	// Szene an die Gr��e des Fensters anpassen
	//////////////////////////////////////////////////////////////////////////
	ReSizeGLScene(width, height);

	//////////////////////////////////////////////////////////////////////////
	// Physikalisches Flugverhalten: K�rper initialisieren
	//////////////////////////////////////////////////////////////////////////
	
	InitAirplane(&f18hornet); // L�dt das 3ds-Modell und speichert es als Vertex-Array

	//////////////////////////////////////////////////////////////////////////
	// Sound initialisieren
	//////////////////////////////////////////////////////////////////////////
	
	sndGeneralSoundInit();

	// Falls DirectSound unterst�tzt wird, DirectSound-Objekt initialisieren
	if((sndGetSoundApiFlags() & DX9SND_OUTPUT) == DX9SND_OUTPUT )
		sndobjectDX9 = sndDX9InitSoundGeneral();
	// Falls NULL zur�ckgegeben wurde Soundflags auf NULL setzen. Dadurch
	// wird der Sound deaktiviert
	if(sndobjectDX9 == NULL)
		sndSetSoundApiFlags(NULL);
	

	return GL_TRUE; // Initialization Went OK
}

//
//	ZWECK: Notwendige Aufr�umarbeiten beim Beenden des Programms
//
//
int KillGLScene() {
	
	//////////////////////////////////////////////////////////////////////////
	// Display-Listen entfernen
	//
	// The glDeleteLists function causes a contiguous group of display lists to be deleted.
	// The list parameter is the name of the first display list to be deleted, and range is the	
	// number of display lists to delete. All display lists d with list = d = list + range � 1
	// are deleted
	//////////////////////////////////////////////////////////////////////////
	devconout("freeing display list memory ...\n");
	glDeleteLists(1, ObjectListID[OBJECT_LIST_COUNT-1]);

	//////////////////////////////////////////////////////////////////////////
	// Objekte aufr�umen
	//////////////////////////////////////////////////////////////////////////
/*	devconout("freeing 3DS memory ...\n");
	for(int counter = 0; counter < OBJECT3DS_COUNT; counter++)
	{
		if(object3dsInfo[counter].object)
			delete object3dsInfo[counter].object;
	}
*/
	devconout("freeing object structure memory ...\n");
	
	//free(cubeInformation);
	free(heightmap);
	free(treemap);
	free(ObjectListID);
	// 3ds-Objekte freigeben
	freeF18Object();
    // Speicher f�r die Kameravektoren freigeben	
	FreeCamera();
	// Heightmap-Speicher freigebn
	// TODO: Heightmap-Speicherverwaltung funktioniert nicht
	//FreeHeightMapVA();

	//////////////////////////////////////////////////////////////////////////
	// Partikel aus dem Partikelsystem freigeben
	//////////////////////////////////////////////////////////////////////////
	devconout("Freeing particles from system...\n");
	FreeParticlesFromSystem(&pool, &em);

	//////////////////////////////////////////////////////////////////////////
	// Soundressourcen freigeben
	//////////////////////////////////////////////////////////////////////////
	
	if((sndGetSoundApiFlags() & OPENAL_OUTPUT) == OPENAL_OUTPUT )
	{
		devconout("OpenAL: cleaning up ... ");
		sndALCleanUp();
		devconout("o.k \n");
	}

	if((sndGetSoundApiFlags() & DX9SND_OUTPUT) == DX9SND_OUTPUT )
	{
		devconout("DirectSound9: cleaning up ... ");
		// Spezifische Objekte aufr�umen (muss in r�ckw�rtiger Reihenfolge geschehen)
		// zuerst die Soundobjekte, dann die COM-Interfaces
		sndDX9CleanUpSndObject(sndobjectDX9[SOUND_ENGINE1_IDX]);
		sndDX9CleanUpSndObject(sndobjectDX9[SOUND_MISSILELAUNCH_IDX]);
		sndDX9CleanUpSndObject(sndobjectDX9[SOUND_BGMUSIC_IDX]);
		sndDX9CleanUp();
		devconout("o.k. \n");
	}

	return 0;
}


// Funktion: ReSizeGLScene
//
// ZWECK: Anpassen der Projektionsmatrix und damit den Viewport
//		  an die neue Fenstergr��e
//
// KOMMENTAR: Wird beim �ndern der Fenstergr��e ben�tigt, um die Perspektive,
//			  und damit den Bereich in dem gerendert wird, anzupassen. 
//			  Es wird zudem jedesmal eine ver�nderte Bildschirmaufl�sung
//			  �berpr�ft, damit die Bewegungsfunktionen weiterhin funtionieren
//			  
GLvoid ReSizeGLScene(GLsizei width, GLsizei height) {
	
	viewport_width  = width;
	viewport_height = height;

	if (height==0)					// Prevent A Divide By Zero By
		height=1;					// Making Height Equal One

	glViewport(0, 0, width, height);// Der Viewport ist das komplette Fenster, es k�nnte auch kleiner sein!
	glMatrixMode(GL_PROJECTION);	// Select The Projection Matrix
	glLoadIdentity();				// Reset The Projection Matrix
	
	// Calculate The Aspect Ratio Of The Window
	// FOV		// Ratio				//  The farthest distance before it stops drawing)
	gluPerspective(FOV,(GLfloat)width/(GLfloat)height,NEAR_PANE,FAR_PANE);
	glMatrixMode(GL_MODELVIEW);		// Select The Modelview Matrix
	glLoadIdentity();				// Reset The Modelview Matrix
	
}

// Funktion: DrawGLScene
//
// ZWECK: Rendern der OpenGL Scenerie
//
// KOMMENTAR:
//
GLvoid DrawGLScene() {

	// Wenn Pause, dann keine Szene rendern
	// Das Programm startet per Definition im Pause-Modus
	// Dieser Modus kann per Tastendurck �ber die Fkt. PauseSimulation()
	// geswitcht werden
	if(pause)
	{
		// Startbildschirm anzeigen
		RenderMainScreen();
		Sleep(100);
		return;
	}

	// Kameravektoren holen
	
	vector3f_t vCamView = GetCameraView();
	vector3f_t vCamPosition = GetCameraPosition();
	vector3f_t vCamUp = GetCameraUp();

	//GetCameraVectors(&vCamPosition, &vCamView, &vCamUp);

	//////////////////////////////////////////////////////////////////////////
	// Flugzeug: Physikalisches Verhalten simulieren
	//////////////////////////////////////////////////////////////////////////
	
	ComputePhysics();

	//////////////////////////////////////////////////////////////////////////
	// Szene initialisieren
	//////////////////////////////////////////////////////////////////////////
	
	glClearColor(0.0f, 0.0f, 0.0f ,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	

	//////////////////////////////////////////////////////////////////////////
	// Kameraposition (Initialisierung der Kamera in InitGL
	// Cameraposition, Richtung, H�henvektor der Kamera
	//////////////////////////////////////////////////////////////////////////
	
	// Statische Soundquelle an die Kameraposition anpassen. Dort wo sich die
	// Kamera befindet ist dann die Soundquelle und wird so f�r den Listener
	// nicht bewegt. (y und z sind vertauscht!!) Eignet sich f�r statische
	// Sounds. Diese Methode ist nicht wirklich zu empfehlen. F�r 2D Sounds
	// empfiehlt sich IDirectSoundBuffer8
	// TODO: Nur tun, wenn sich die Kamera bewegt!
	if( (sndGetSoundApiFlags() & DX9SND_OUTPUT) == DX9SND_OUTPUT )
	{
		sndDX9SetSoundSourcePos(sndobjectDX9[SOUND_BGMUSIC_IDX],
								vCamPosition.x,
								vCamPosition.z-3.0f,
								vCamPosition.y);
		// Listenerposition einstellen
		sndDX9SetListenerPos(vCamPosition.x,vCamPosition.z, vCamPosition.y);
	}

	gluLookAt(vCamPosition.x, vCamPosition.y, vCamPosition.z,	
				  vCamView.x,	  vCamView.y,     vCamView.z,	
				    vCamUp.x,	    vCamUp.y,       vCamUp.z);

	//////////////////////////////////////////////////////////////////////////
	// Flugzeugmodell als Vertex-Array rendern
	//////////////////////////////////////////////////////////////////////////

//	renderF18Object();
	
	/*glPushMatrix();
		glTranslatef(130, 3, -10);
		renderF18Object();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(-130, 2, -10);
		renderF18Object();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(300, 4, -20);
		renderF18Object();
	glPopMatrix();
	*/

	//////////////////////////////////////////////////////////////////////////
	// Partikelsystem testen
	//////////////////////////////////////////////////////////////////////////
	
	//UpdateEmitter(&pool, &em);
	//RenderParticleSystem(&em);

	//static int i = 0;
	//if(++i > 100)
	//{
	//	devconout("particle in pool: %d, out: %d\n", pool.particlein, pool.particleout);
	//	i = 0;
	//}

	//////////////////////////////////////////////////////////////////////////
	//cube_t zeichnen
	//////////////////////////////////////////////////////////////////////////

/*	
//	int cubecount;
	for(int cubecount = 0; cubecount < 11; cubecount ++)
	{
		// Liste aufrufen und Objekt rendern
		glPushMatrix();
		glTranslatef(cubecount - 5.0f ,0,0);
		glCallList( ObjectListID[CUBE_LIST_IDX] );
		glPopMatrix();
	}
*/	
	//////////////////////////////////////////////////////////////////////////
	// Skybox zeichnen
	//////////////////////////////////////////////////////////////////////////

	// Sky-Box
	
	DrawSkyBox(0,	0, 0,				// Zentrum der Skybox
			   HEIGHTMAP_SIZE*100, HEIGHTMAP_SIZE*100, HEIGHTMAP_SIZE*100,		// Ausdehnung auf allen Achsen
			   SKYAHEAD_TEXTURE_IDX,	// Texturinformationen zum Mappen
			   SKYBACK_TEXTURE_IDX,
			   SKYDOWN_TEXTURE_IDX,
			   SKYLEFT_TEXTURE_IDX,
			   SKYRIGHT_TEXTURE_IDX,
			   SKYUP_TEXTURE_IDX);
	
	//////////////////////////////////////////////////////////////////////////
	//  Testweise ein Quadrat zeichnen
	//////////////////////////////////////////////////////////////////////////

	// TODO: Billboarding Exact fixen

	/*
	SetBillboardCamPos(&vCamPosition);
	BillboardCylindricalExactBegin(0.0f, 0.0f, -1.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureInfo[LENSF1_TEXTURE_IDX].textureID);
	glBegin(GL_QUADS);
	//glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, -1.0f);  
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, -1.0f);  
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);  
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 1.0f, -1.0f); 
	glEnd();
	BillboardEnd();
	glDisable(GL_TEXTURE_2D);
	*/
	
	//////////////////////////////////////////////////////////////////////////
	// Height-Map zeichnen
	//////////////////////////////////////////////////////////////////////////

	// Height-Map aus der Display-Liste heraus generieren
	glCallList( ObjectListID[HEIGHTMAP_LIST_IDX] );
	//DrawHeightMapShaded(heightmap);
	//DrawHeightMapTextured(heightmap, GROUND_TEXTURE_IDX);
	//renderHeightmapVA(textureInfo[GROUND_TEXTURE_IDX].textureID);

	//////////////////////////////////////////////////////////////////////////
	// B�ume rendern
	//////////////////////////////////////////////////////////////////////////

	// Treemap rendern
	// UND
	// Viewfrustum neu berechnen
	// TODO: Eigentlich inperformant weil das jede Szene durchgef�hrt wird
	//		 Besser immer dann, wenn sich die Modelviewmatrix �ndert...
	//		 Da dies hier aber durch glScalef und glTranslatef auch immer der Fall
	//		 ist, kann das (vorerst) ignoriert werden
	
	if(!render_wireframe)
	{
		glPushMatrix();
		// Skalieren der Treemap
		glScalef(20.0f, 20.0f, 20.0f);
		// Zentrieren der Treemap
		glTranslatef( -HEIGHTMAP_SIZE / 2, -256.0f, -HEIGHTMAP_SIZE / 2);

		GetFrustumPanesMS(mvspace_planes);

		// Nur rendern wenn nicht im Wireframemodus
		
		RenderTreeMap(treemap, heightmap,
				textureInfo[TREE1_TEXTURE_IDX].textureID,
				textureInfo[TREE2_TEXTURE_IDX].textureID, 
				textureInfo[TREE3_TEXTURE_IDX].textureID, 
				textureInfo[TREE4_TEXTURE_IDX].textureID,
				mvspace_planes);
		glPopMatrix();
	} // if

/*	
	// Zweite Heightmap links anf�gen (-x)
	glPushMatrix();
	glScalef(20.0f, 20.0f, 20.0f);
	// Zentrieren der Landschaft
	glTranslatef( -HEIGHTMAP_SIZE * 1.5, -256.0f, -HEIGHTMAP_SIZE / 2);
	glCallList( ObjectListID[HEIGHTMAP_LIST_IDX] );
	glPopMatrix();
	
	// Dritte Heightmap
	glPushMatrix();
	glScalef(20.0f, 20.0f, 20.0f);
	// Zentrieren der Landschaft
	glTranslatef( HEIGHTMAP_SIZE * 0.5f, -256.0f, -HEIGHTMAP_SIZE / 2);
	glCallList( ObjectListID[HEIGHTMAP_LIST_IDX] );
	glPopMatrix();
*/

	//////////////////////////////////////////////////////////////////////////
	// Rakete abfeuern (wenn SPACE gedr�ckt wurde)
	//////////////////////////////////////////////////////////////////////////
	
	// TODO: Das Partikelsystem der Rakete: Emmitterinitialisierung ausgliedern!
	RenderMissile();

	//////////////////////////////////////////////////////////////////////////
	// HUD zeichnen
	/////////////////////////////////////////////////////////////////////////

	DrawHUD();

	//////////////////////////////////////////////////////////////////////////
	// Lens Flare Effekt rendern
	//////////////////////////////////////////////////////////////////////////
	
	// Aktuelles Frustum im Model-Space ermitteln
	GetFrustumPanesMS(mvspace_planes);
	
	// Wenn der Mittelpunkt der Lens Flare sichtbar ist, dann dort rendern
	if(isPointVisibleOnPlane(mvspace_planes, NewVector(1000,300,100)))
		RenderLensFlare(NewVector(1000,300,100));


	//////////////////////////////////////////////////////////////////////////
	// Framerate berechnen und anzeigen
	//////////////////////////////////////////////////////////////////////////
	
	CalcFrameRate();

	//////////////////////////////////////////////////////////////////////////
	// Profiling-Informationen ausgeben
	//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG	
	char out[50];
	StringCbPrintfA(out, sizeof(out)-1, "rendered verticies: %d/frame", profile_getVertexCount());
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX],
		viewport_height, 230, FONT_HEIGHT * 2,out);
	profile_resetVertexCount();
#endif

	//////////////////////////////////////////////////////////////////////////
	// Drahtgittermodell statusinformation anzeigen
	//////////////////////////////////////////////////////////////////////////
	
	if(render_wireframe)
	{
		glColor3f(1,0,0);
		glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX],
					viewport_height, 30, FONT_HEIGHT * 3,"wireframe : on");
		glColor3f(1, 1, 1);
	}
	else
	{
		glColor3f(1,1,1);
		glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX],
			viewport_height, 30, FONT_HEIGHT * 3,"wireframe : off");
		glColor3f(1, 1, 1);
	}


	// Sicherstellen, dass alles in diesem Durchgang gerendert und bearbeitet wird
	glFlush();
}

//
//	Rakete mit Schweif rendern (bei Abschuss per SPACE-Taste)
//
//	1.) Aktuelle Position des Flugzeugs ermitteln
//  2.) Richtungsvektor bestimmen
//  3.) Rakete in diese Richtung abfeuern und Position aktualisieren
//
GLvoid RenderMissile()
{
	if(!fireMissile)
		return;

	if(phi >= 360.0f)
		phi = 0.0f;

	glPushMatrix();
		
		// rudiment�re Bewegung der Rakete
		glTranslatef(3.0f, -2.0f, posz+=3.0f);
		
		glPushMatrix();
			glRotatef(phi+=2.0f, 0,0,1.0f);
			//glScalef(0.5f, 0.5f, 0.5f);
			//glRotatef(90.0f, 0, 1.0f, 0);
			//glTranslatef(0,0,2.0f);
//			object3dsInfo[MISSILE_OBJECT3DS_IDX].object->display();
			// Schweif als Partikelsystem
			glPushMatrix();
				glTranslatef(0,0,-4.0f);	
				UpdateEmitter(&pool, &em);
				// TODO: Partikelsystem generalisieren
				RenderParticleSystem(&em, GL_TRUE, textureInfo[ROCKETTRAIL_TEXTURE_IDX].textureID );
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}
//
// Wird aus der PollKeyState aufgerufen um einen Abschuss durchzuf�hren
//
GLvoid LaunchMissile()
{
	fireMissile = GL_TRUE;
	
	// Sound abspielen
	if((sndGetSoundApiFlags() & DX9SND_OUTPUT) == DX9SND_OUTPUT )
	{
		vector3f_t vCamPosition = GetCameraPosition();
		// Achtung y/z-Achse vertauscht in DirectSound!
		sndDX9PlaySoundAt(NewVector(vCamPosition.x,vCamPosition.z, vCamPosition.y), // Listener (entspricht Kamera)
			NewVector(0,0,0),							   // Soundquelle (Flugzeug)
			sndobjectDX9[SOUND_MISSILELAUNCH_IDX],		   // Soundobjekt -> Rakete
			FALSE);										   // Kein Loop
	}
}

//
// NAME: DrawHUD()
//
// ZWECK: Head-Up-Display auf der Projektionsmatrix zeichnen
//	   	  auf dieser kann dann normal mit 2D-Screenkoordinaten gezeichnet werden
//		  Objekte die so gezeichnet werden, erscheinen, als w�ren sie direkt auf die
//		  Bildschirmfl�che gezeichnet. Sie sind immer auf den Betrachter ausgerichtet
//		  Um 3D-Objektkoordinaten auf 2D Screenkoordinaten umzurechnen gibt es
//		  die glu-Funktion gluProject()

GLvoid DrawHUD()
{
	// Modelviewmatrix sichern
	glPushMatrix();
	// In den Projektionsmodus wechseln
	glMatrixMode(GL_PROJECTION);	
	// Projektionsmatrix sichern
	glPushMatrix();				
		// Einheitsmatrix laden
		glLoadIdentity();	
		// Neue Projektion ist Orthogonal �ber den kompletten viewport-Bereich
		glOrtho( 0, viewport_width, viewport_height, 0, 0, 1 );	 //links,rechts,unten,oben,near,far
		// In den Modelview-Mode wechseln
		glMatrixMode(GL_MODELVIEW);		
		// Einheitsmatrix laden (WICHTIG: Vorher muss die alte Matrix gesichert werden!)
		glLoadIdentity();

		// Vorbereiten f�r das Alpha-Blending
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.1f);
		
		// Cockpit der F18 rendern (�ber den ganzen Viewport)
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureInfo[F18COCKPIT_TEXTURE_IDX].textureID);
		glBegin(GL_QUADS);
			// Display the top left point of the 2D image
			glTexCoord2f(0.0f, 1.0f);	profile_glVertex2f(0, 0);
			// Display the bottom left point of the 2D image
			glTexCoord2f(0.0f, 0.0f);	profile_glVertex2f(0, (float)viewport_height);
			// Display the bottom right point of the 2D image
			glTexCoord2f(1.0f, 0.0f);	profile_glVertex2f((float)viewport_width, (float)viewport_height);
			// Display the top right point of the 2D image
			glTexCoord2f(1.0f, 1.0f);	profile_glVertex2f((float)viewport_width, 0);
		glEnd();
		// Zur�cksetzen der Stati
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		// In den Projektionsmatrix-Modus wechseln
		glMatrixMode( GL_PROJECTION );			
	// Vorher gesicherte 3D-Perspektivische Projektion zur�ckholen
	glPopMatrix();
	// In den Modelview-Mode wechseln
	glMatrixMode( GL_MODELVIEW );
	// Gesicherte Matrix zur�ckholen
	glPopMatrix();

	//////////////////////////////////////////////////////////////////////////
	// Status des Flugzeugs ausgeben
	//////////////////////////////////////////////////////////////////////////

	char hud_txt[128] = "";

	// Gr�ne Schrift
	glColor3f(0.0f,1.0f,0.0f);


	// Schub
	StringCbPrintfA(hud_txt,sizeof(hud_txt)-1,"Thrust %4.1f", f18hornet.thrust_force);
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX],
		viewport_height, viewport_width/2, viewport_height / 5, hud_txt );
	// Speed
	StringCbPrintfA(hud_txt,sizeof(hud_txt)-1,"Speed %4.1f", f18hornet.speed);
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX],
		viewport_height, viewport_width/2, viewport_height / 5 + FONT_HEIGHT, hud_txt );
	// Altitude
	StringCbPrintfA(hud_txt,sizeof(hud_txt)-1,"Altitude %4.1f", f18hornet.position.z);
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX],
		viewport_height, viewport_width/2, viewport_height / 5 + FONT_HEIGHT * 2, hud_txt );
	glColor3f(1, 1, 1);
	// Koordinaten
	StringCbPrintfA(hud_txt,sizeof(hud_txt)-1,"x=%4.1f,y=%4.1f,z=%4.1f", f18hornet.position.x, f18hornet.position.y, f18hornet.position.z);
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX],
		viewport_height, viewport_width/2, viewport_height / 5 + FONT_HEIGHT * 3, hud_txt );
	// Euler angles
	StringCbPrintfA(hud_txt,sizeof(hud_txt)-1,"Pitch %4.1f, Yaw %4.1f, Roll %4.1f", f18hornet.euler_angle.y, f18hornet.euler_angle.z, f18hornet.euler_angle.x);
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX],
		viewport_height, viewport_width/2, viewport_height / 5 + FONT_HEIGHT * 4, hud_txt );

	// Steuerungshilfe
	glColor3f(1.0f, 1.0f, 0.0f);
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX], viewport_height, 10, 100, "CONTROLS:");
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX], viewport_height, 10, 100 + FONT_HEIGHT, "W / UP: Pitch Down");
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX], viewport_height, 10, 100 + FONT_HEIGHT * 2, "S / DOWN: Pitch Up");
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX], viewport_height, 10, 100 + FONT_HEIGHT * 3, "A / D: Roll Left/Right");
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX], viewport_height, 10, 100 + FONT_HEIGHT * 4, "PGUP/PGDN: Thrust");
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX], viewport_height, 10, 100 + FONT_HEIGHT * 5, "M: Toggle Mouse Stick");
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX], viewport_height, 10, 100 + FONT_HEIGHT * 6, "P: Pause/Resume");
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX], viewport_height, 10, 100 + FONT_HEIGHT * 7, "R: Reset Sim");

	// Auf wei zurckschalten
	glColor3f(1, 1, 1);


	/*
	// Einstellungen f�r die 2D Darstellung anpassen
	glClear(GL_DEPTH_BUFFER_BIT);
	// Transparenz (Alpha-Kanal) aktivieren
	glEnable(GL_BLEND);
	// Art des Blendings einstellen
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	// Tiefen Buffer read-only
	glDepthMask(GL_FALSE);
	// Beleuchtung ist hier nicht erforderlich
	glDisable(GL_LIGHTING);
	// auch kein Nebel
	glDisable(GL_FOG);
	// Die 2D Elemente sollen immer im Vordergrund sein
	glDisable(GL_DEPTH_TEST);
	
	glPushMatrix();
	glLoadIdentity();
	// Projektionsmatrix als aktuelle Matrix definieren
	glMatrixMode(GL_PROJECTION);
	// Matrixstack benutzen um die aktuelle Projektion nicht zu zerst�ren
	glPushMatrix();
		glLoadIdentity();
		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureInfo[F18COCKPIT_TEXTURE_IDX].textureID);
		glBegin(GL_QUADS);
			glColor4ub(73, 74, 131, 128);
			glTexCoord2f(0.0f, 0.0f); profile_glVertex2f(0.0f, 0.0f);  
			glTexCoord2f(1.0f, 0.0f); profile_glVertex2f(0.15f, 0.0f);  
			glTexCoord2f(1.0f, 1.0f); profile_glVertex2f(0.15f, 0.1f);  
			glTexCoord2f(0.0f, 1.0f); profile_glVertex2f(0.0f, 0.1f); 
		glEnd();
	
	glPopMatrix();

	// Wieder auf die Modelview-Matrix wechsekn
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// Z�r�cksetzen der angepassten Parameter f�r 2D Darstellung	
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);		// write-enabled
	glEnable(GL_LIGHTING);
	glEnable(GL_FOG);
	glDisable(GL_BLEND);
	*/
}

//
// Startbildschirm anzeigen (�hnlich DrawHUD nur ohne Alpha-Blending)
//
GLvoid RenderMainScreen()
{
	// Modelviewmatrix sichern
	glPushMatrix();
	// In den Projektionsmodus wechseln
	glMatrixMode(GL_PROJECTION);	
	// Projektionsmatrix sichern
	glPushMatrix();				
	// Einheitsmatrix laden
	glLoadIdentity();	
	// Neue Projektion ist Orthogonal �ber den kompletten viewport-Bereich
	glOrtho( 0, viewport_width, viewport_height, 0, 0, 1 );	 //links,rechts,unten,oben,near,far
	// In den Modelview-Mode wechseln
	glMatrixMode(GL_MODELVIEW);		
	// Einheitsmatrix laden (WICHTIG: Vorher muss die alte Matrix gesichert werden!)
	glLoadIdentity();

	// Vorbereiten f�r das Alpha-Blending
	/*glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1);
	*/

	// Cockpit der F18 rendern (�ber den ganzen Viewport)
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureInfo[MAINSCREEN_IDX].textureID);
	glBegin(GL_QUADS);
	// Display the top left point of the 2D image
	glTexCoord2f(0.0f, 1.0f);	profile_glVertex2f(0, 0);
	// Display the bottom left point of the 2D image
	glTexCoord2f(0.0f, 0.0f);	profile_glVertex2f(0, (float)viewport_height);
	// Display the bottom right point of the 2D image
	glTexCoord2f(1.0f, 0.0f);	profile_glVertex2f((float)viewport_width, (float)viewport_height);
	// Display the top right point of the 2D image
	glTexCoord2f(1.0f, 1.0f);	profile_glVertex2f((float)viewport_width, 0);
	glEnd();
	// Zur�cksetzen der Stati
/*	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
*/
	// In den Projektionsmatrix-Modus wechseln
	glMatrixMode( GL_PROJECTION );			
	// Vorher gesicherte 3D-Perspektivische Projektion zur�ckholen
	glPopMatrix();
	// In den Modelview-Mode wechseln
	glMatrixMode( GL_MODELVIEW );
	// Gesicherte Matrix zur�ckholen
	glPopMatrix();
}

//
// NAME: ResetSimulation
//
// ZWECK: Simulation wieder in den Ausgangszustand versetzen
//		  Wird auch von InitGL() verwendet
//
//	TODO: Nach dem Reset stimmt etwas mit dem Bewegungswinkel der Maussteuerung nicht...
// 
GLvoid ResetSimulation()
{
	// Kamera wieder in die Ursprungsposition fahren
	SetCameraVectors(NewVector(0,0,30.0f), NewVector(0,0,0), NewVector(0.0,1.0f,0) );

	// Spielerinteraktion: Werte zur�cksetzen
	fireMissile = GL_FALSE;
	posz=0.0f;
	phi=0.0f;

	// Physikalische Simulation zur�cksetzen
	InitAirplane(&f18hornet);

	// Sound initialisieren
	if((sndGetSoundApiFlags() & DX9SND_OUTPUT) == DX9SND_OUTPUT )
	{
		// Zuh�rer wieder auf die Kameraposition setzen
		sndDX9SetListenerPos( 0,0, 0);
	}

}

GLvoid PauseSimulation()
{
	pause = !pause;
}

//
//	NAME : CalcFrameRate
//
//	ZWECK: Framerate berechnen und auf RC ausgeben
GLvoid CalcFrameRate()
{
	/*static float		fps = 0.0f;			// Frames Per Second
	float				curr_time = 0.0f;	// Aktuelle System-Zeit in Sek.
	static float		last_time = 0.0f;	// Zeit um eine Sekunde zu bestimmen
	static float		frame_time = 0.0f;  // Zeit bei Beginn des letzten Frames

	// Ein Puffer f�r die FPS
	static char buffer[256] = "";					

	// Aufl�sung des Timers auf 1 ms setzen (Standard ist 5 ms unter WinXP/2000)
	MMRESULT result = timeBeginPeriod(1);
	assert(result ==  TIMERR_NOERROR );
	curr_time = timeGetTime() / 1000.0f;
	result = timeEndPeriod(1);
	assert(result ==  TIMERR_NOERROR );

	if(frame_time != 0.0f)
	{
		// FPS berechnen
		fps = 1.0f / (curr_time - frame_time);
	}

	// Neue Ausgabe erst dann in den Buffer schreiben, wenn
	// min. eine Sekunde vergangen ist.
	if(curr_time - last_time > 1.0f)
	{
		last_time = curr_time;

		StringCbPrintf(buffer, sizeof(buffer),
			"fps : %4.3f", fps);
	}

	// Ausgeben
	glColor3f(1, 1, 1);
	glDrawText2D(ObjectListID[VERDANAFONT_LIST_IDX],
		viewport_height, 30,FONT_HEIGHT * 2, buffer);
	glColor3f(1,0,0);

	frame_interval = curr_time - frame_time;
	frame_time = curr_time;
	*/	

	// Framerate-Variablen
/*	static time_t time_diff;			// Zeitunterschied zwischen Progstart und jezt

	static unsigned long framecount = 0;// Z�hlt die gezeichneten Frames 

	char text_message[256];				// Auszugebende Nachricht durch GDI-Fkt
	char buffer[256];					// Ein Puffer f�r die FPS

	//////////////////////////////////////////////////////////////////////////
	// Framerate berechnen
	//////////////////////////////////////////////////////////////////////////

	framecount++;
	time_diff = time(NULL) - time_progstart;

	// Framerate-String zusammenbauen
	StringCbPrintf(text_message, sizeof(text_message)-1,
		"frames : %lu, sec : %lu",
		framecount, time_diff);

	// DIV/0 abfangen
	if(time_diff != 0) 
	{
		StringCbPrintf(buffer, sizeof(buffer)-1,
			", fps : %3.3f", (float)framecount / (float)time_diff); 
		StringCbCat(text_message, sizeof(text_message)-1,
			buffer);
	}


	glColor3f(1, 1, 1);
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX],
				 viewport_height, 30,FONT_HEIGHT * 2, text_message);
	glColor3f(1,0,0);
	
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX],
		viewport_height, 30, FONT_HEIGHT,(LPCSTR) glGetString(GL_RENDERER));
	glColor3f(1, 1, 1);
*/
	
	
	/* Ermitteln der aktuellen Programmlaufzeit in Millisekunden und hoch-
	z�hlen des Framecounters. */

	int time = GetTickCount();
	static int framecount;
	static int  timebase;
	static char fpstext[50] = "fps = n/a";

	framecount++;

	// Erst wenn eine Sekunde verstichen ist, werden die FPS augegeben
	if( time - timebase > 1000 )
	{
		StringCbPrintfA( fpstext, sizeof(fpstext)-1,"fps = %4.2f", framecount*1000.0/(time - timebase) );

		timebase   = time;
		framecount = 0;
	}
	
	glColor3f(1, 1, 1);
	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX],
		viewport_height, 30,FONT_HEIGHT * 2, fpstext);
	glColor3f(1,0,0);

	glDrawText2D(ObjectListID[ARIALFONT_LIST_IDX],
		viewport_height, 30, FONT_HEIGHT,(LPCSTR) glGetString(GL_RENDERER));
	glColor3f(1, 1, 1);

}

//
// NAME : ToggleWireFrameView
//
// ZWECK: �ndert die Darstellung der Polygone.
//
GLvoid ToggleWireFrameView() 
{
	if(render_wireframe == GL_FALSE)
	{
		// Render the triangles in wire frame mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		render_wireframe = !render_wireframe;
	}
	else
	{
		// Render the triangles in fill mode		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
		render_wireframe = !render_wireframe;
	}
}

//////////////////////////////////////////////////////////////////////////
// Flugzeugsteuerung
//////////////////////////////////////////////////////////////////////////


GLvoid IncThrust()
{
	IncThrust(&f18hornet);
}

GLvoid DecThrust()
{
	DecThrust(&f18hornet);
}

GLvoid PitchUp()
{
	PitchUp(&f18hornet);
}

GLvoid PitchDown()
{
	PitchDown(&f18hornet);
}

GLvoid RollLeft()
{
	RollLeft(&f18hornet);
}

GLvoid RollRight()
{
	RollRight(&f18hornet);
}

GLvoid ZeroRudder()
{
	ZeroRudder(&f18hornet);
}

GLvoid ZeroAilerons()
{
	ZeroAilerons(&f18hornet);
}
GLvoid ZeroElevators()
{
	ZeroElevators(&f18hornet);
}
//
// NAME: ToggleVSync
//
// ZWECK: Vertikale Zeilenfrequenz des Monitors mit der Bildwiederholfrequenz
//		  synchronisieren, wenn VSync = an. Dadurch wird die Framerate begrenzt
//		  Das geschieht direkt auf Treiberebene, durch eine OpenGL Extension, die
//		  der IHV (Independent Harware Vendor) zur Verf�gung stellt.
//
GLvoid ToggleVSync()
{
	// Adresse der Extension-Funktion im Treiber holen.
	wglSwapIntervalEXT_Func wglSwapIntervalEXT;
	wglSwapIntervalEXT = wglSwapIntervalEXT_Func(wglGetProcAddress("wglSwapIntervalEXT"));
	// Umschalten
	if(wglSwapIntervalEXT)
		wglSwapIntervalEXT(0);
}

GLvoid ComputePhysics()
{
	//////////////////////////////////////////////////////////////////////////
	// Physikalische Simulation durchf�hren
	//////////////////////////////////////////////////////////////////////////

	// Zeit�nderung bestimmen
	static DWORD OldTime = 0;
	static float total_time = 0;

	DWORD NewTime = timeGetTime();
	if (OldTime == 0) OldTime = NewTime;
	float dt = (float) (NewTime - OldTime)/1000.0f;
	if (dt > 0.016f) dt = 0.016f;
	if (dt < 0.001f) dt = 0.001f;
	OldTime = NewTime;
	total_time += dt;

	// In Echtzeit simulieren
	StepSimulationStdEuler(dt, &f18hornet);

	// Kameraposition anpassen (damit der Eindruck einer Bewegung entsteht
	// Hier muss auf die unterschiedlichen Koordinatensysteme R�cksicht genommen werden
	//
	// Body -> GL
	//	-y   | x
	//  -x   | z
	//   z   | y
	// 

	// Kamerablickpunkt anpassen (Die Kamera blickt entlang der Ausrichtung des Flugzeugs)
	vector3f_t vz = GetBodyZAxisVector(&f18hornet); // Zeigt im Body-System nach oben
	vector3f_t vx = GetBodyXAxisVector(&f18hornet); // Zeigt im Body-System nach vorne
	
	vector3f_t camPos = NewVector(-f18hornet.position.y, f18hornet.position.z, -f18hornet.position.x);
	vector3f_t camView = NewVector(camPos.x - vx.y, camPos.y + vx.z, camPos.z - vx.x);
	vector3f_t camUp = NewVector(-vz.y, vz.z, -vz.x);

	SetCameraVectors(camPos, camView, camUp);

	/*SetCameraOrientation(	-vx.y, vx.z, vx.x, 
	-vz.y, vz.z, vz.x);
	*/
	
	// Wenn Str�mungsabriss, Sound abspielen
	if(f18hornet.stall)
	{
		if( (sndGetSoundApiFlags() & DX9SND_OUTPUT) == DX9SND_OUTPUT )
		{
			vector3f_t vCamPosition = GetCameraPosition();

			// Abspielen des Stall-Sounds. Falls er schon gespielt wird, wird FALSE zur�ckgegeben
			if(!sndDX9PlaySoundAt(NewVector(vCamPosition.x,
									 	 vCamPosition.z-5.0f,
										 vCamPosition.y),				// soundquelle
							  NewVector(vCamPosition.x,
										 vCamPosition.z,
										 vCamPosition.y),			// listener
							  sndobjectDX9[SOUND_STALL_IDX],	// soundobjekt
							  FALSE))
			{
				// Da der Sound bereits gespielt wird, braucht nur die Position angepasst
				// zu werden
				sndDX9SetSoundSourcePos(sndobjectDX9[SOUND_STALL_IDX],
										vCamPosition.x,
										vCamPosition.z-5.0f,
										vCamPosition.y );
				sndDX9SetListenerPos( vCamPosition.x,
									  vCamPosition.z,
									  vCamPosition.y);
			}
		}
	}

}

// Die folgende Funktion dient zur Darstellung eines Linseneffektes wenn sich die Kamera
// in die Richtung der Sonne dreht. Hierbei werden einzelne Linsengrafiken, hintereinander
// aufgereit auf einem Direktionvektor zwischen Sonnenmittelpunkt und Kameraposition als
// 2D Fl�che dargestellt, die per Billboardfunktionalit�t auf den Betrachter ausgerichtet
// werden (Billboarding wird hier dadurch erreicht, dass direkt auf der Projektionsmatrix
// gerendert wird). 

void RenderLensFlare(vector3f_t position)
{
	// TODO: Diese Funktion aufr�umen und anpassen
	//		 au�erdem: Blending kl�ren, denn es funktioniert NUR so
	//		 Vielleicht liegt es daran, dass auf der Projektionsmatrix gerendert wird...
	//		 Au�erdem werden sogar BMPs mit schwarzem Hintergrund transparent gerendert...

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_ONE, GL_ONE);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	
	/*glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1);
	*/
	glEnable(GL_TEXTURE_2D);
	
	// Erzeugen diverser Matrixzenvariablen
	GLdouble viewMatrix[16];
	GLdouble projMatrix[16];
	GLdouble sx, sy;
	GLint viewPort[4];

	// Ermitteln der aktuellen Matrizen
	glGetDoublev(GL_MODELVIEW_MATRIX, viewMatrix);
	glGetDoublev( GL_PROJECTION_MATRIX, projMatrix );
	glGetIntegerv(GL_VIEWPORT, viewPort);

	// Ermitteln der Objektkoordinaten als 2D Screen-Koordinaten
	double objScreenX, objScreenY, objScreenZ;
	gluProject(position.x, position.y, position.z,
			   viewMatrix, projMatrix, viewPort,
			   &objScreenX, &objScreenY, &objScreenZ);

	// Die ermittelten Screenkoordinaten werden jetzt auf die Viewport Ma�e angepasst
	objScreenX = objScreenX/(float)viewPort[2]*2.0-1.0; 
	objScreenY = objScreenY/(float)viewPort[2]*2.0-0.7; 

	// Ermitteln des Vektors zwischen Bildschirmmitte und Objektkoordinaten
	sx = 0.0-objScreenX;
	sy = 0.0-objScreenY;

	// Ermitteln der L�nge des Vektors und erstellen eines Skalierungsfaktors
	double length = sqrt( (sx * sx) + (sy * sy) + (0 * 0) );
	double scale  = 0.5;

	// Erst wenn die Objektkoordinaten in den Viewbereich kommen, wird ein Darstellen des Linseneffektes als
	// 2D Linseneffekt m�glich, daher wird zn�chst gepr�ft ob der Bereich erreicht ist : 

	if(objScreenX <= viewport_width)
	{
		// Ist er erreicht, dan wechseln wir auf die Projektionsmatrix
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		// Ermitteln der Objektposition f�r den 1. Linseneffekt (gro�e Ringlinse)
		sx = sx*length;
		sy = sy*length;

		glBindTexture(GL_TEXTURE_2D, textureInfo[LENSF1_TEXTURE_IDX].textureID);
		glBegin(GL_QUADS);
		glColor4ub(73, 74, 131, 255);
		glTexCoord2f(0.0f, 0.0f); profile_glVertex2d(sx-0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 0.0f); profile_glVertex2d(sx+0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 1.0f); profile_glVertex2d(sx+0.5*scale, sy+0.5*scale);
		glTexCoord2f(0.0f, 1.0f); profile_glVertex2d(sx-0.5*scale, sy+0.5*scale);
		glEnd();

		sx = 0.0-objScreenX;
		sy = 0.0-objScreenY;

		// Ermitteln der Objektposition und skalieren des 2. Linseneffektes (Gl�hen)
		sx = sx*length/1.1;
		sy = sy*length/1.1;
		scale = 0.05f;

		glBindTexture(GL_TEXTURE_2D, textureInfo[LENSF4_TEXTURE_IDX].textureID);
		glColor4ub(110, 135, 201, 255);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); profile_glVertex2d(sx-0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 0.0f); profile_glVertex2d(sx+0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 1.0f); profile_glVertex2d(sx+0.5*scale, sy+0.5*scale);
		glTexCoord2f(0.0f, 1.0f); profile_glVertex2d(sx-0.5*scale, sy+0.5*scale);
		glEnd();


		sx = 0.0-objScreenX;
		sy = 0.0-objScreenY;


		// Ermitteln der Objektposition und skalieren des 3. Linseneffektes (gef�llte Ringlinse)
		sx = sx*length/2;
		sy = sy*length/2;
		scale = 0.05f;

		glBindTexture(GL_TEXTURE_2D, textureInfo[LENSF1_TEXTURE_IDX].textureID);
		glColor4ub(74, 115, 107, 255);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); profile_glVertex2d(sx-0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 0.0f); profile_glVertex2d(sx+0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 1.0f); profile_glVertex2d(sx+0.5*scale, sy+0.5*scale);
		glTexCoord2f(0.0f, 1.0f); profile_glVertex2d(sx-0.5*scale, sy+0.5*scale);
		glEnd();


		sx = 0.0-objScreenX;
		sy = 0.0-objScreenY;

		// Ermitteln der Objektposition und skalieren des 4. Linseneffektes (gef�llte Ringlinse)
		sx = sx*length/3;
		sy = sy*length/3;
		scale = 0.1f;

		glBindTexture(GL_TEXTURE_2D, textureInfo[LENSF2_TEXTURE_IDX].textureID);
		glColor4ub(85, 55, 17, 255);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); profile_glVertex2d(sx-0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 0.0f); profile_glVertex2d(sx+0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 1.0f); profile_glVertex2d(sx+0.5*scale, sy+0.5*scale);
		glTexCoord2f(0.0f, 1.0f); profile_glVertex2d(sx-0.5*scale, sy+0.5*scale);
		glEnd();

		sx = 0.0-objScreenX;
		sy = 0.0-objScreenY;

		// Ermitteln der Objektposition und skalieren des 5. Linseneffektes (Gl�hen)
		sx = sx*length/8;
		sy = sy*length/8;
		scale = 0.05;

		glBindTexture(GL_TEXTURE_2D, textureInfo[LENSF2_TEXTURE_IDX].textureID);
		glColor4ub(57, 67, 165, 255);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); profile_glVertex2d(sx-0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 0.0f); profile_glVertex2d(sx+0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 1.0f); profile_glVertex2d(sx+0.5*scale, sy+0.5*scale);
		glTexCoord2f(0.0f, 1.0f); profile_glVertex2d(sx-0.5*scale, sy+0.5*scale);
		glEnd();

		sx = 0.0-objScreenX;
		sy = 0.0-objScreenY;

		// Ermitteln der Objektposition und skalieren des 6. Linseneffektes (Stern)
		sx = sx*-(length/2);
		sy = sy*-(length/2);
		scale = 0.1;

		glBindTexture(GL_TEXTURE_2D, textureInfo[LENSF3_TEXTURE_IDX].textureID);
		glColor4ub(48, 56, 77, 255);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); profile_glVertex2d(sx-0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 0.0f); profile_glVertex2d(sx+0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 1.0f); profile_glVertex2d(sx+0.5*scale, sy+0.5*scale);
		glTexCoord2f(0.0f, 1.0f); profile_glVertex2d(sx-0.5*scale, sy+0.5*scale);
		glEnd();

		sx = 0.0-objScreenX;
		sy = 0.0-objScreenY;

		// Ermitteln der Objektposition und skalieren des 7. Linseneffektes (gef�llte Ringlinse)
		sx = sx*-length;
		sy = sy*-length;
		scale = 0.1;

		glBindTexture(GL_TEXTURE_2D, textureInfo[LENSF2_TEXTURE_IDX].textureID);
		glColor4ub(62, 25, 43, 255);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); profile_glVertex2d(sx-0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 0.0f); profile_glVertex2d(sx+0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 1.0f); profile_glVertex2d(sx+0.5*scale, sy+0.5*scale);
		glTexCoord2f(0.0f, 1.0f); profile_glVertex2d(sx-0.5*scale, sy+0.5*scale);
		glEnd();

		sx = 0.0-objScreenX;
		sy = 0.0-objScreenY;

		// Ermitteln der Objektposition und skalieren des 7. Linseneffektes (gef�llte Ringlinse)
		sx = sx*-(length*2);
		sy = sy*-(length*2);
		scale = 0.2;

		glBindTexture(GL_TEXTURE_2D, textureInfo[LENSF2_TEXTURE_IDX].textureID);
		glColor4ub(114, 103, 83, 255);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); profile_glVertex2d(sx-0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 0.0f); profile_glVertex2d(sx+0.5*scale, sy-0.5*scale);
		glTexCoord2f(1.0f, 1.0f); profile_glVertex2d(sx+0.5*scale, sy+0.5*scale);
		glTexCoord2f(0.0f, 1.0f); profile_glVertex2d(sx-0.5*scale, sy+0.5*scale);
		glEnd();

		// Wechsel auf 3D Matrixmode
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}


	// Z�r�cksetzen der angepassten Parameter f�r Darstellung	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	
	// Farbe auf Wei� setzen
	glColor3f(1, 1, 1);
}


