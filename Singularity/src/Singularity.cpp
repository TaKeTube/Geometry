#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Mesh/HexahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>
#include <iostream>
#include <string>
#include "Singularity.hpp"

/*
 * findSingularity()
 * DESCRIPTION: find singular lines of a ovm hex mesh
 * INPUT: mMesh - OpenVolumeMesh hex mesh
 *        innerSingularity - reference to a vector of edges to be filled with inner singular edges
 *        boundarySingularity - reference to a vector of edges to be filled with boundary singular edges
 * OUTPUT: none
 * RETURN: number of all singular edges
 */
int findSingularity(
    OpenVolumeMesh::GeometricPolyhedralMeshV3f &mMesh,
    std::vector<Edge> &innerSingularity,
    std::vector<Edge> &boundarySingularity)
{
    int numInnerSingularity = 0;
    int numBoundarySingularity = 0;

    std::cout << "Finding singularities... " << std::endl;

    /* find singular edges by iterating edges of the mesh */
    for (auto e_iter = mMesh.edges_begin(); e_iter != mMesh.edges_end(); ++e_iter)
    {
        bool isBoundary = mMesh.is_boundary(*e_iter);
        int valence = mMesh.valence(*e_iter);

        Edge e = mMesh.edge(e_iter.cur_handle());

        /* valence in ovm of a edge is the number of adjacent faces, which is different with our definition */
        /* therefore, for inner regular edge, valence(ovm) is 4, for boundary regular edge, valence(ovm) is 3 */
        if (!isBoundary && valence != 4)
        {
            innerSingularity.push_back(e);
            numInnerSingularity++;
        }
        else if (isBoundary && valence != 3)
        {
            boundarySingularity.push_back(e);
            numBoundarySingularity++;
        }
    }

    std::cout << "Number of inner singularities: " << numInnerSingularity << std::endl;
    std::cout << "Number of boundary singularities: " << numBoundarySingularity << std::endl;

    /* return total number of singular edges */
    return numInnerSingularity + numBoundarySingularity;
}
