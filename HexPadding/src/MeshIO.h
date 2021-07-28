#ifndef MESH_IO_H
#define MESH_IO_H

#include "hpMesh.h"

int meshReader(const char* fname, HexPadding::Mesh& mesh);
void vtkReader(const char* fname , HexPadding::Mesh& mesh);
void objReader(const char* fname , HexPadding::Mesh& mesh);
void vtkWriter(const char* fname , HexPadding::Mesh& mesh);
void vtkScalarWriter(const char* fname, HexPadding::Mesh& mesh, std::vector<int> scalarVec);

#endif