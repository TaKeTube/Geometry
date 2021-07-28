#include <cstring>
#include <cstdio>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "MeshIO.h"
#include "hpMesh.h"
#include "HexPadding.h"

int main(int argc, char **argv)
{
    char *input_file = NULL;
    char *output_file = NULL;
    char *target_file = NULL;
    char default_file[] = "../data/64cube.vtk";
    char default_target_file[] = "../data/64cube_target.txt";
    bool smooth_flag = false;
    bool mark_flag = false;
    bool help_flag = false;

    /* 
     *  A standard command: 
     *      ./Padding.exe -i "../data/rod.vtk" -o "smoothed_padded_rod.vtk" -t "../data/rod_target.txt" -s -m
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
        else if (!strcmp(argv[i], "-t"))
        {
            i++;
            assert(i < argc);
            target_file = argv[i];
        }
        else if (!strcmp(argv[i], "-s"))
        {
            smooth_flag = true;
        }
        else if (!strcmp(argv[i], "-m"))
        {
            mark_flag = true;
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
        std::cout << "Pad selected hex elements of a hex mesh." << std::endl;
        std::cout << "HELP:" << std::endl;
        std::cout << "-i arg : input vtk file, arg: input vtk file name, default: ../data/64cube.vtk" << std::endl;
        std::cout << "-o arg : output vtk file, arg: output vtk file name, default: output.vtk" << std::endl;
        std::cout << "-t arg : target cell indexes in txt file, arg: target txt file name, default: ../data/64cube_target.txt" << std::endl;
        std::cout << "-s     : smooth the padded mesh" << std::endl;
        std::cout << "-m     : output mesh with padded element marked using scalar 1" << std::endl;
        std::cout << "-h     : help" << std::endl;
        return 0;
    }

    HexPadding::Mesh mesh = HexPadding::Mesh();

    // std::vector<size_t> MarkedC = {
    //     2, 6, 7
    // };

    /* read marked cells from file */
    std::ifstream fin((target_file == NULL)?default_target_file:target_file);
    std::vector<size_t> MarkedC;
    int cIdx;

    std::cout<<"Read target cells from file...\nMarked Cells:"<<std::endl;
    while(fin>>cIdx) {
        std::cout<<cIdx<<" ";
        MarkedC.push_back(cIdx);
    }
    fin.close();
    std::cout<<std::endl;

    /* Padding */
    /* read mesh file */
    std::cout << "Read mesh from file..." << std::endl;
    if (!meshReader((input_file == NULL) ? default_file : input_file, mesh))
    {
        /* padding */
        HexPadding::padding(mesh, MarkedC, smooth_flag, mark_flag);
        /* output the processed mesh */
        std::string out_name = (output_file == NULL) ? "output.vtk" : output_file;
        vtkWriter(out_name.c_str(), mesh);
        /* mark padded cells if needed */
        if (mark_flag)
        {
            std::vector<int> PaddedFlag(mesh.C.size(), 0);
            for (size_t cIdx : mesh.PaddedC)
                PaddedFlag.at(cIdx) = 1;
            vtkScalarWriter(out_name.c_str(), mesh, PaddedFlag);
        }
    }
    else
    {
        /* fail to read file */
        std::cout << "Fail to read file" << std::endl;
    }
    return 0;
}