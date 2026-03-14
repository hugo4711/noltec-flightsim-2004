//////////////////////////////////////////////////////////////////////////
// Funktionen die zum Initialisieren und Verwalten des OpenGL-Renderings notwendig sind
// werden hier definiert

#ifndef _GLSCENE_H
#define _GLSCENE_H

#define FOV						45.0f
#define NEAR_PANE				1.0f
#define FAR_PANE				1000000.0f

#include "math/vectormath.h"

//////////////////////////////////////////////////////////////////////////
// Initialisierungsfunktion
//////////////////////////////////////////////////////////////////////////

GLboolean InitGL(HDC hDC, GLsizei width, GLsizei height);
// Aufräumarbeiten beim Beenden des Programms
int KillGLScene(GLvoid); 
// Größe des Renderkontext anpassen, wird bei WM_SIZE benötigt
GLvoid ReSizeGLScene(GLsizei width, GLsizei height); 

//////////////////////////////////////////////////////////////////////////
// Rendering- und Funktionen die die Szene beeinflussen
//////////////////////////////////////////////////////////////////////////

// Haupt-Rendering Funktion
GLvoid DrawGLScene();
// HUD auf der Projection-Matrix zeichnen
GLvoid DrawHUD();
// Hauptbildschirm anzeigen
GLvoid RenderMainScreen();
// Kamera bewegen
GLvoid MoveCamera(float speed);
// Kamera mit der Maus bewegen
GLvoid SetCameraViewByMouse();
// Kamera wieder in den Ursprung setzen
GLvoid ResetSimulation();
GLvoid PauseSimulation();
// View rotieren
GLvoid RotateCamera(float angle, float x, float y, float z);
// Quaternionenkamera
GLvoid RotateCameraQ(float angle,float x, float y, float z);
// Framerate berechnen
GLvoid CalcFrameRate();
// Drahtgittermodell ein/aus
GLvoid ToggleWireFrameView();
// Rakete abfeuern
GLvoid LaunchMissile();
GLvoid RenderMissile();
// Lensflare
void RenderLensFlare(vector3f_t position);

// Flugzeugsteuerung
GLvoid ComputePhysics();
GLvoid IncThrust();
GLvoid DecThrust();
GLvoid PitchUp();
GLvoid PitchDown();
GLvoid ZeroRudder();
GLvoid ZeroAilerons();
GLvoid ZeroElevators();
GLvoid RollRight();
GLvoid RollLeft();

// vsync notwendige Funktionen
typedef BOOL (APIENTRY * wglSwapIntervalEXT_Func)(int);
GLvoid ToggleVSync();

#endif /* _GLSCENE_H */