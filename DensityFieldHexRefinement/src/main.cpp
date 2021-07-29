#include <iostream>
#include "FieldAdaptiveRefine.h"
#include "MeshIO.h"
#include "HexEval/HexEval.h"

using namespace Eigen;

int main(int argc, char **argv)
{
    char *input_file = NULL;
    char *output_file = NULL;
    std::string density_metric = "";
    std::string refine_method = "";
    char default_file[] = "../data/cad.vtk";
    bool smooth_flag = false;
    bool mark_flag = false;
    bool eval_flag = false;
    bool help_flag = false;
    int iterNum = 3;

    /*
   *  A standard command:
   *      ./HexRefinement.exe -i "../data/rod.vtk" -o "refined_rod.vtk" -s -e
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
        else if (!strcmp(argv[i], "-d"))
        {
            i++;
            assert(i < argc);
            density_metric = argv[i];
        }
        else if (!strcmp(argv[i], "-r"))
        {
            i++;
            assert(i < argc);
            refine_method = argv[i];
        }
        else if (!strcmp(argv[i], "-t"))
        {
            i++;
            assert(i < argc);
            iterNum = std::stoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-s"))
        {
            smooth_flag = true;
        }
        else if (!strcmp(argv[i], "-m"))
        {
            mark_flag = true;
        }
        else if (!strcmp(argv[i], "-e"))
        {
            eval_flag = true;
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
        std::cout << "Refine hex mesh according to a density field." << std::endl;
        std::cout << "HELP:" << std::endl;
        std::cout << "-i arg : input vtk file, arg: input vtk file name, default: ../data/cad.vtk" << std::endl;
        std::cout << "-o arg : output vtk file, arg: output vtk file name, default: output.vtk" << std::endl;
        std::cout << "-d arg : density metric, arg: len/vol, default: len" << std::endl;
        std::cout << "-r arg : refine method, arg: padding/trivial, default: padding" << std::endl;
        std::cout << "-t arg : number of iterations, arg: number of iterations, default: 3" << std::endl;
        std::cout << "-s     : smooth the padded mesh" << std::endl;
        std::cout << "-m     : output mesh with padded element marked using scalar 1" << std::endl;
        std::cout << "-e     : evaluate the results, output field of the result mesh, reference & difference field" << std::endl;
        std::cout << "-h     : help" << std::endl;
        return 0;
    }

    /* decide density metric */
    HexEval::DensityMetric densityMetric;
    if (density_metric.empty() || density_metric == "len")
    {
        /* using length metric */
        std::cout << "Using edge-length density metric" << std::endl;
        densityMetric = HexEval::EDGE_LENGTH_METRIC;
    }
    else if (density_metric == "vol")
    {
        /* using volume metric (1/V) */
        densityMetric = HexEval::VOLUME_METRIC;
        std::cout << "Using volume density metric" << std::endl;
    }
    else
    {
        std::cout << "wrong density metric argument." << std::endl;
        return -1;
    }

    /* decide refine method */
    RefineMethod refineMethod;
    if (refine_method.empty() || refine_method == "padding")
    {
        /* using padding method */
        std::cout << "Using padding refine method" << std::endl;
        refineMethod = PADDING_REFINE;
    }
    else if (refine_method == "trivial")
    {
        /* using trivial method */
        refineMethod = TRIVIAL_REFINE;
        std::cout << "Using trivial refine method" << std::endl;
    }
    else
    {
        std::cout << "wrong refine method argument." << std::endl;
        return -1;
    }

    Matrix3Xd V;
    MatrixXi C;
    std::function<double(Eigen::Vector3d)> densityField;
    std::function<Eigen::Matrix3d(Eigen::Vector3d)> anisotropicDensityField;

    if (!meshReader((input_file == NULL) ? default_file : input_file, V, C))
    {
        // densityField = [](Vector3d v)
        //                {return 258*exp(-v.squaredNorm()/2)/sqrt(2*M_PI);});
        // densityField = [](Vector3d v)
        //                { return 195 * sin(v.y() * 3); }, PADDING_REFINE);
        densityField = [](Vector3d v)
                       { return 50 * (1 + sin(v.y() * 3)); };
        
        FieldAdaptiveRefine(
                            V,
                            C,
                            densityField,
                            refineMethod,
                            densityMetric,
                            iterNum,
                            smooth_flag,
                            mark_flag,
                            eval_flag
                            );

        vtkWriter((output_file == NULL) ? "output.vtk" : output_file, V, C);
    }
}