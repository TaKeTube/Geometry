#include <iostream>

#include "MeshIO.h"
#include "HexEval.h"

using namespace Eigen;

int main(int argc, char **argv)
{
    char *input_file = NULL;
    char *output_file = NULL;
    char *density_metric = NULL;
    char default_file[] = "../data/cad.vtk";

    /* 
     *  A standard command: 
     *      ./HexEval.exe -input "../data/rod.vtk" -output "rod_output.vtk" -density "len"
     */
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-input"))
        {
            i++;
            assert(i < argc);
            input_file = argv[i];
        }
        else if (!strcmp(argv[i], "-output"))
        {
            i++;
            assert(i < argc);
            output_file = argv[i];
        }
        else if (!strcmp(argv[i], "-density"))
        {
            i++;
            assert(i < argc);
            density_metric = argv[i];
        }
        else
        {
            printf("Error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }

    Matrix3Xd V;
    MatrixXi C;
    HexEval::HexEvaluator evaluator;
    std::string densityMetricStr = density_metric;
    HexEval::DensityMetric densityMetric;

    if (density_metric == NULL || densityMetricStr == "len")
    {
        std::cout << "Evaluate density field... \nUsing edge-length density metric" << std::endl;
        densityMetric = HexEval::EDGE_LENGTH_METRIC;
    }
    else if (densityMetricStr == "vol")
    {
        densityMetric = HexEval::VOLUME_METRIC;
        std::cout << "Evaluate density field... \nUsing volume density metric" << std::endl;
    }
    else if (densityMetricStr == "anisotropic")
    {
        densityMetric = HexEval::ANISOTROPIC_METRIC;
        std::cout << "Evaluate density field... \nUsing anisotropic density metric" << std::endl;
    }
    else
    {
        std::cout << "wrong density metric argument." << std::endl;
        return -1;
    }

    std::string outputString = (output_file == NULL) ? "output.vtk" : output_file;
    outputString.erase(outputString.length()-4, 4);

    if (!meshReader((input_file == NULL) ? default_file : input_file, V, C))
    {
        evaluator.setRefDensityField([](Vector3d v)
                                    { return 50 * (1 + sin(v.y() * 3)); });
        if (densityMetric == HexEval::ANISOTROPIC_METRIC)
        {
            std::function<Eigen::Matrix3d(Eigen::Vector3d)> isotropicField = [](Vector3d v)
            { return Eigen::MatrixXd::Identity(3, 3); };
            evaluator.setAnisotropicDensityField(isotropicField);
        }
        if (evaluator.EvalDensityField(V, C, densityMetric) == -1)
            return -1;
        vtkWriter((outputString + ".vtk").c_str(), V, C, evaluator.GetDensityField());
        vtkWriter((outputString + "_reffield.vtk").c_str(), V, C, evaluator.GetRefDensityField(V, C));
        vtkWriter((outputString + "_difffield.vtk").c_str(), V, C, evaluator.GetDiffDensityField(V, C));
    }
}