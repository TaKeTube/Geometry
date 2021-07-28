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
    bool diff_flag = false;
    bool ref_flag = false;
    bool help_flag = false;

    /* 
     *  A standard command: 
     *      ./HexEval.exe -i "../data/rod.vtk" -o "rod_output.vtk" -m "len" -r -d
     */
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-i"))
        {
            i++;
            assert(i < argc);
            input_file = argv[i];
        }
        else if (!strcmp(argv[i], "-o"))
        {
            i++;
            assert(i < argc);
            output_file = argv[i];
        }
        else if (!strcmp(argv[i], "-m"))
        {
            i++;
            assert(i < argc);
            density_metric = argv[i];
        }
        else if (!strcmp(argv[i], "-d"))
        {
            diff_flag = true;
        }
        else if (!strcmp(argv[i], "-r"))
        {
            ref_flag = true;
        }
        else if (!strcmp(argv[i], "-h"))
        {
            help_flag = true;
        }
        else
        {
            printf("Error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }

    /* output help info */
    if (help_flag)
    {
        std::cout << "Evaluate hex density." << std::endl;
        std::cout << "HELP:" << std::endl;
        std::cout << "-i arg : input, arg: input file name, default: ../data/cad.vtk" << std::endl;
        std::cout << "-o arg : output, arg: output file name, default: output.vtk" << std::endl;
        std::cout << "-m arg : density metric, arg: len/vol/anisotropic, default: len" << std::endl;
        std::cout << "-r     : output reference field if setted" << std::endl;
        std::cout << "-d     : output the difference between the actual density field and the reference field" << std::endl;
        std::cout << "-h     : help" << std::endl;
        return 0;
    }

    Matrix3Xd V;
    MatrixXi C;
    HexEval::HexEvaluator evaluator;
    std::string densityMetricStr = density_metric;
    HexEval::DensityMetric densityMetric;

    if (density_metric == NULL || densityMetricStr == "len")
    {
        /* using length metric */
        std::cout << "Evaluate density field... \nUsing edge-length density metric" << std::endl;
        densityMetric = HexEval::EDGE_LENGTH_METRIC;
    }
    else if (densityMetricStr == "vol")
    {
        /* using volume metric (1/V) */
        densityMetric = HexEval::VOLUME_METRIC;
        std::cout << "Evaluate density field... \nUsing volume density metric" << std::endl;
    }
    else if (densityMetricStr == "anisotropic")
    {
        /* 
         * using anisotropic metric described in 
         * Automated refinement of conformal quadrilateral and hexahedral meshes 
         *                              - Tchon KF, Dompierre J, Camarero R
         */
        densityMetric = HexEval::ANISOTROPIC_METRIC;
        std::cout << "Evaluate density field... \nUsing anisotropic density metric" << std::endl;
    }
    else
    {
        std::cout << "wrong density metric argument." << std::endl;
        return -1;
    }

    std::string outputString = (output_file == NULL) ? "output.vtk" : output_file;
    outputString.erase(outputString.length() - 4, 4);

    if (!meshReader((input_file == NULL) ? default_file : input_file, V, C))
    {
        /* set reference density field */
        evaluator.setRefDensityField([](Vector3d v)
                                     { return 50 * (1 + sin(v.y() * 3)); });
        
        /* if using anisotropic metric, use another type of density field */
        if (densityMetric == HexEval::ANISOTROPIC_METRIC)
        {
            std::function<Eigen::Matrix3d(Eigen::Vector3d)> isotropicField = [](Vector3d v)
            { return Eigen::MatrixXd::Identity(3, 3); };
            evaluator.setAnisotropicDensityField(isotropicField);
        }
        
        /* evaluate the field */
        if (evaluator.EvalDensityField(V, C, densityMetric) == -1)
            return -1;
        
        /* output density field */
        vtkWriter((outputString + ".vtk").c_str(), V, C, evaluator.GetDensityField());
        
        /* if using anisotropic metric, there is no such thing called reference and difference field */
        if (densityMetric == HexEval::ANISOTROPIC_METRIC) return 0;

        /* output refernce field */
        if (ref_flag)
            vtkWriter((outputString + "_reffield.vtk").c_str(), V, C, evaluator.GetRefDensityField(V, C));
        
        /* output difference field */
        if (diff_flag)
            vtkWriter((outputString + "_difffield.vtk").c_str(), V, C, evaluator.GetDiffDensityField(V, C));
    }
}