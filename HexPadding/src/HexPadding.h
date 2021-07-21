#ifndef HEX_PADDING_H
#define HEX_PADDING_H

#include "hpMesh.h"

void padding(Mesh& mesh, std::vector<size_t> &markedC);
void volSmoothing(Mesh& mesh);
void volSmoothingSubmesh(Mesh& mesh, Mesh& subMesh);
void volLaplacianSmoothing(Mesh& mesh);
void volLaplacianSmoothingSubmesh(Mesh& mesh, Mesh& subMesh);
void getSubMesh(Mesh &mesh, Mesh &subMesh, std::vector<size_t> &markedC);

#endif
