#include "Singularity.hpp"
#include "vtkIO.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(){
    const string s = "test.ovm";

    /* open a mesh file */
    OpenVolumeMesh::GeometricPolyhedralMeshV3f mMesh;
    OpenVolumeMesh::IO::FileManager fm;
    std::cout << fm.readFile(s, mMesh) << std::endl;
    
    vector<Edge> innerSingularity;
    vector<Edge> boundarySingularity;

    findSingularity(mMesh, innerSingularity, boundarySingularity);

    // Vec3f v1 = mMesh.vertex(e.from_vertex());
    // Vec3f v2 = mMesh.vertex(e.from_vertex());

    return 0;
}