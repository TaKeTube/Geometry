#ifndef HEX_PADDING_H
#define HEX_PADDING_H

#include "hpMesh.h"

void padding(Mesh& mesh, std::vector<size_t> markedC);
void volSmoothingUsingCells(Mesh& mesh);
void volSmoothingSubmeshUsingCells(Mesh& mesh, Mesh& subMesh);
void volSmoothingUsingEdges(Mesh& mesh);
void volSmoothingSubmeshUsingEdges(Mesh& mesh, Mesh& subMesh);
void getSubMesh(Mesh &mesh, Mesh &subMesh, std::vector<size_t> &markedC);

#endif
