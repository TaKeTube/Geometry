#ifndef MESH_IO_H
#define MESH_IO_H

#include "hpMesh.h"

int meshReader(const char* fname, Mesh& mesh);
void vtkReader(const char* fname , Mesh& mesh);
void objReader(const char* fname , Mesh& mesh);
void vtkWriter(const char* fname , Mesh& mesh);

#endif