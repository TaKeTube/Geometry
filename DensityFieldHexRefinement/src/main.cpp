#include <iostream>

#include "MeshIO.h"
#include "FieldAdaptiveRefine.h"

using namespace Eigen;

int main(int argc, char **argv){
    char* input_file = NULL;
    char* output_file = NULL;
    // char* refine_file = NULL;
    char default_file[] = "../data/cad.vtk";
    // char default_refine_file[] = "../data/cad_refine.txt";

    /* 
     *  A standard command: 
     *      ./HexRefinement.exe -input "../data/rod.vtk" -output "refined_rod.vtk" -refine "../data/rod_refine.txt"
     */
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i],"-input")) {
            i++; assert (i < argc); 
            input_file = argv[i];
        } else if (!strcmp(argv[i],"-output")) {
            i++; assert (i < argc); 
            output_file = argv[i];
        // } else if (!strcmp(argv[i],"-refine")) {
        //     i++; assert (i < argc); 
        //     refine_file = argv[i];
        } else {
            printf ("Error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }

    Matrix3Xd V;
    MatrixXi C;

    if(!meshReader((input_file == NULL)?default_file:input_file, V, C)){
        FieldAdaptiveRefine(V, C, [](Vector3d v){return 258*exp(-v.squaredNorm()/2)/sqrt(2*M_PI);});
        vtkWriter((output_file == NULL)?"output.vtk":output_file, V, C);
    }
}