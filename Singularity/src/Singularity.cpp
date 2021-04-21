#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Mesh/HexahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>
#include <iostream>
#include <string>
#include "Singularity.hpp"

int findSingularity(
    OpenVolumeMesh::GeometricPolyhedralMeshV3f &mMesh,
    std::vector<Edge> &innerSingularity,
    std::vector<Edge> &boundarySingularity)
{
    int numInnerSingularity = 0;
    int numBoundarySingularity = 0;

    std::cout << "Finding singularities... " << std::endl;

    for (auto e_iter = mMesh.edges_begin(); e_iter != mMesh.edges_end(); ++e_iter)
    {
        bool isBoundary = mMesh.is_boundary(*e_iter);
        int valence = mMesh.valence(*e_iter);

        Edge e = mMesh.edge(e_iter.cur_handle());

        if (!isBoundary && valence != 4)
        {
            innerSingularity.push_back(e);
            numInnerSingularity++;
        }
        else if (isBoundary && valence != 2)
        {
            boundarySingularity.push_back(e);
            numBoundarySingularity++;
        }
    }

    std::cout << "Number of inner singularities: " << numInnerSingularity << std::endl;
    std::cout << "Number of boundary singularities: " << numBoundarySingularity << std::endl;

    return numInnerSingularity + numBoundarySingularity;
}
