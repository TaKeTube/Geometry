#ifndef SINGULARITY_HPP
#define SINGULARITY_HPP

#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Mesh/HexahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>
#include <iostream>
#include <string>

typedef OpenVolumeMesh::VectorT<float, 3> Vec3f;
typedef OpenVolumeMesh::OpenVolumeMeshEdge Edge;

int findSingularity(
    OpenVolumeMesh::GeometricPolyhedralMeshV3f &mMesh,
    std::vector<Edge> &innerSingularity,
    std::vector<Edge> &boundarySingularity);

#endif