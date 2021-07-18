#include "HexPadding.h"

using namespace std;

void padding(Mesh &m, vector<size_t> &markedC){
    Mesh markedMesh;

    /* generate mesh from marked cells */
    getMarkedMesh(markedC, markedMesh);

    /* get normals of vertexes of markedMesh */
    markedMesh.getGeometryInfo();
    markedMesh.getSurfaceNormal();
}