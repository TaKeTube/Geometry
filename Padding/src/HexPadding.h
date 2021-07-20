#ifndef HEX_PADDING_H
#define HEX_PADDING_H

#include "hpMesh.h"

void padding(Mesh& mesh, std::vector<size_t> &markedC);
void getMarkedSubMesh(Mesh &mesh, Mesh &subMesh, std::vector<size_t> &markedC);

#endif
