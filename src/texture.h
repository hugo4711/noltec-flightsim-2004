//////////////////////////////////////////////////////////////////////////
// Texturinformationen
//////////////////////////////////////////////////////////////////////////

#ifndef _TEXTURE_H
#define _TEXTURE_H

#define	LDR_TEXTURE_CNT		22	// Anzahl der zu ladenden Texturen (TextureID[])
// muss der Anzahl der definierten Namen entsprechen!

// Konstanten, die den Index im TextureID[] Array entsprechen,
// um die Texturen entsprechend dort abzulegen und per Index zu addressieren.

// TODO: Für diese Texturkonstanten müsste ein Konzept existieren, mit dem per Konfig-File
//	     alle notwendigen Dateien geladen werden (gilt auch für 3DS-Objekte sowie Sound)

// Texturen für Testzwecke
#define CRATE_TEXTURE_IDX		0				 // Index der Textur "cube_t" im Array TextureID[]
#define CRATE_TEXTURE_NAME		"img/crate.bmp" // Pfadangabe zur cube_t-Textur
// Height-Map Texturen
#define HEIGHT_MAP_IDX			1
#define HEIGHT_MAP_NAME			"img/geomaps/heightmap.raw"
#define GROUND_TEXTURE_IDX		2
#define GROUND_TEXTURE_NAME		"img/geomaps/ground.tga"
// Skybox Texturen
#define SKYAHEAD_TEXTURE_IDX	3
#define SKYAHEAD_TEXTURE_NAME	"img/skybox/ahead.bmp"
#define SKYBACK_TEXTURE_IDX		4
#define SKYBACK_TEXTURE_NAME	"img/skybox/back.bmp"
#define SKYRIGHT_TEXTURE_IDX	5
#define SKYRIGHT_TEXTURE_NAME	"img/skybox/right.bmp"
#define SKYLEFT_TEXTURE_IDX		6
#define SKYLEFT_TEXTURE_NAME	"img/skybox/left.bmp"
#define SKYUP_TEXTURE_IDX		7
#define SKYUP_TEXTURE_NAME		"img/skybox/up.bmp"
#define SKYDOWN_TEXTURE_IDX		8
#define SKYDOWN_TEXTURE_NAME	"img/skybox/down.bmp"
// OpenGL Logo
#define GLLOGO_TEXTURE_IDX		9
#define GLLOGO_TEXTURE_NAME		"img/gllogo.tga"
// Cockpit der F18
#define F18COCKPIT_TEXTURE_IDX	10
#define F18COCKPIT_TEXTURE_NAME	"img/f18cockpit.tga"
// Treemap
#define TREE_MAP_IDX			11
#define TREE_MAP_NAME			"img/geomaps/treemap.raw"
// Trees
#define TREE1_TEXTURE_IDX		12
#define TREE1_TEXTURE_NAME		"img/geomaps/tree1.tga"
#define TREE2_TEXTURE_IDX		13
#define TREE2_TEXTURE_NAME		"img/geomaps/tree2.tga"
#define TREE3_TEXTURE_IDX		14
#define TREE3_TEXTURE_NAME		"img/geomaps/tree3.tga"
#define TREE4_TEXTURE_IDX		15
#define TREE4_TEXTURE_NAME		"img/geomaps/tree4.tga"
#define MAINSCREEN_IDX			16
#define MAINSCREEN_NAME			"img/background/main_bg.tga"
#define LENSF1_TEXTURE_IDX		17
#define LENSF1_NAME				"img/lensf1.tga"
#define LENSF2_TEXTURE_IDX		18
#define LENSF2_NAME				"img/lensf2.tga"
#define LENSF3_TEXTURE_IDX		19
#define LENSF3_NAME				"img/lensf3.tga"
#define LENSF4_TEXTURE_IDX		20
#define LENSF4_NAME				"img/lensf4.tga"
#define ROCKETTRAIL_TEXTURE_IDX	21
#define ROCKETTRAIL_NAME		"img/rockettrail.tga"

// Definiert den Dateityp
// Wichtig: Es funktionieren nur Typen, die auch im Loader implementiert sind!
typedef enum {FMT_BMP,		// Bitmap Textur
			  FMT_TGA,
			  FMT_RAW} ldr_texturefmt_t;	

// Informationen, die zum Laden der Texturen benötigt werden
// Name der Textur, Typ der Textur und ID der Textur. MAX_PATH_LENGTH ist in
// stdafx.h definiert
typedef struct 
{
	char				textureName[MAX_PATH_LENGTH];
	ldr_texturefmt_t	textureType;
	GLuint				textureID;
	GLboolean			mipmapped;						// soll die Textur gemipmapt werden?
} ldr_textureinf_t;



// CLAMP_TO_EDGE muss manuell definiert werden, damit die Übergänge
// zwischen den Skybox-Texturen nahtlos sind

// The GL_CLAMP_TO_EDGE constant was introduced in OpenGL version 1.2.
// If you use an older version, it won't be supported, there's just GL_REPEAT and GL_CLAMP.
// If your GL header files are from before version 1.2, your compiler will complain about the
// unknown constant GL_CLAMP_TO_EDGE. You can get newer header files (or #define this constant as 0x812F),
// but this of course doesn't mean your OpenGL implementation will support it. 
//
// You can get the version of your OpenGL implementation as a string value via: 
// glGetString(GL_VERSION)
// If this returns "1.2"or higher, you're in luck and you may use the GL_CLAMP_TO_EDGE value. 
//
// If you're pass the GL_CLAMP_TO_EDGE constant to an OpenGL version that doesn't support it, 
// it will be silently ignored.  Which means you'll still be using the default repeat mode
// texture wrapping. (http://home.planet.nl/~monstrous/skybox.html)

#define GL_CLAMP_TO_EDGE	0x812F

#endif /* _TEXTURE_H */