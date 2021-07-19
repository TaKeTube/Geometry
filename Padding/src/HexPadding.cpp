#include "HexPadding.h"

using namespace std;

void padding(Mesh &m, vector<size_t> &markedC){
    Mesh markedSubMesh;

    /* generate mesh from marked cells */
    getMarkedSubMesh(markedC, markedSubMesh);

    /* get normals of vertexes of markedMesh */
    markedSubMesh.getSurface();
    markedSubMesh.getGeometryInfo();
    markedSubMesh.getSurfaceNormal(m);
    markedSubMesh.getSurfaceAvgLen(m);
}