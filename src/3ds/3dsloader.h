/////////////////////////////////////////////////////////
// 3ds Loader Header-File

#ifndef _3DSLOADER_H_
#define _3DSLOADER_H_

// Definieren des Mesh-Radius
#define MESH_RADIUS 100.0

float *Load3DS(char *name,int *num_vertex);
// Funktionen zum translieren
void mesh_size(float *vertex,int num_vertex,float *min,float *max);
float mesh_radius(float *vertex,int num_vertex);
void mesh_translate(float *vertex,int num_vertex,float x,float y,float z);
void mesh_scale(float *vertex,int num_vertex,float scale);

#endif /* _3DSLOADER_H_ */