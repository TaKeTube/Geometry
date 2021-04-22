#include <iostream>
#include <string>
#include "Singularity.hpp"
#include "vtkIO.hpp"

using namespace std;

int main(int argc, char **argv){
    char* inputFname = NULL;
    char* innerOutputFname = NULL;
    char* boundaryOutputFname = NULL;
    char defaultInputFname[] = "../data/bunny.ovm";
    char defaultInnerOutputFname[] = "innerSingularity.vtk";
    char defaultBoundaryOutputFname[] = "boundarySingularity.vtk";

    /* 
     *  A standard command: 
     *      ./FlatAngleTerminator.exe -input "../data/cube.obj" -output "processed_cube.vtk"
     */
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i],"-input")) {
            i++; assert (i < argc); 
            inputFname = argv[i];
        } else if (!strcmp(argv[i],"-iout")) {
            i++; assert (i < argc); 
            innerOutputFname = argv[i];
        } else if (!strcmp(argv[i],"-bout")) {
            i++; assert (i < argc); 
            boundaryOutputFname = argv[i];
        } else {
            printf ("Error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }

    /* open a mesh file */
    OpenVolumeMesh::GeometricPolyhedralMeshV3f mMesh;
    OpenVolumeMesh::IO::FileManager fm;
    std::cout << fm.readFile(inputFname == NULL ? defaultInputFname : inputFname, mMesh) << std::endl;

    vector<Edge>    innerSingularity;
    vector<Edge>    boundarySingularity;

    findSingularity(mMesh, innerSingularity, boundarySingularity);

    vtkSingularitiesWriter( mMesh, innerSingularity, 
                            innerOutputFname == NULL ? defaultInnerOutputFname : innerOutputFname);
    vtkSingularitiesWriter( mMesh, boundarySingularity, 
                            boundaryOutputFname == NULL ? defaultBoundaryOutputFname : boundaryOutputFname);

    return 0;
}