#include <iostream>

#include "MeshIO.h"
#include "HexEval.h"

using namespace Eigen;

int main(int argc, char **argv){
    char* input_file = NULL;
    char* output_file = NULL;
    char default_file[] = "../data/cad.vtk";

    /* 
     *  A standard command: 
     *      ./HexRefinement.exe -input "../data/rod.vtk" -output "rod_output.vtk"
     */
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i],"-input")) {
            i++; assert (i < argc); 
            input_file = argv[i];
        } else if (!strcmp(argv[i],"-output")) {
            i++; assert (i < argc); 
            output_file = argv[i];
        } else {
            printf ("Error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }

    Matrix3Xd V;
    MatrixXi C;
    HexEval::HexEvaluator evaluator;

    if(!meshReader((input_file == NULL)?default_file:input_file, V, C)){
        // std::function<Eigen::Matrix3d (Eigen::Vector3d)> isotropicField = [](Vector3d v){return Eigen::MatrixXd::Identity(3, 3);};
        // evaluator.setAnisotropicDensityField(isotropicField);
        evaluator.EvalDensityField(V, C, HexEval::EDGE_LENGTH_METRIC);
        vtkWriter((output_file == NULL)?"output.vtk":output_file, V, C, evaluator.GetDensityField());
    }
}