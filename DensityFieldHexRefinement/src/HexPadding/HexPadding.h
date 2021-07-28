#ifndef HEX_PADDING_H
#define HEX_PADDING_H

#include "hpMesh.h"

namespace HexPadding
{
    void padding(Mesh &mesh, std::vector<size_t> markedC, bool smooth, bool markPadded);
    void volSmoothingUsingCells(Mesh &mesh);
    void volSmoothingSubmeshUsingCells(Mesh &mesh, Mesh &subMesh);
    void volSmoothingUsingVerts(Mesh &mesh);
    void volSmoothingSubmeshUsingVerts(Mesh &mesh, Mesh &subMesh);
    void getSubMesh(Mesh &mesh, Mesh &subMesh, std::vector<size_t> &markedC);
}

#endif
