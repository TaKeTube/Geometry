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

    /* 
     *  A standard command: 
     *      ./Padding.exe -s -input "../data/rod.vtk" -output "smoothed_padded_rod.vtk" -target "../data/rod_target.txt"
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
        else if (!strcmp(argv[i], "-target"))
        {
            i++;
            assert(i < argc);
            target_file = argv[i];
        }
        else if (!strcmp(argv[i], "-s"))
        {
            smooth_flag = true;
        }
        else
        {
            printf("Error with command line argument %d: '%s'\n", i, argv[i]);
            assert(0);
        }
    }

    Mesh mesh = Mesh();

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
        padding(mesh, MarkedC, smooth_flag);
        /* output the processed mesh */
        vtkWriter((output_file == NULL) ? "output.vtk" : output_file, mesh);
    }
    else
    {
        /* fail to read file */
        std::cout << "Fail to read file" << std::endl;
    }
    return 0;
}