#include <cstring>
#include <cstdio>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "MeshIO.h"
#include "Mesh.h"
#include "RefineTest.h"
#include "global.hpp"

// #define TEST

int main(int argc, char **argv){
    char* input_file = NULL;
    char* output_file = NULL;
    char* refine_file = NULL;
    char default_file[] = "../data/cad.vtk";
    char default_refine_file[] = "../data/cad_refine.txt";

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
        } else if (!strcmp(argv[i],"-refine")) {
            i++; assert (i < argc); 
            refine_file = argv[i];
        } else {
            printf ("Error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }

#ifndef TEST

    Mesh mesh = Mesh();

    /* for test */
    // std::vector<size_t> selectedV = {
    //     197, 322, 69, 709, 730, 720, 25, 126
    // };

    /* read selected vertexes from file */
    std::ifstream fin((refine_file == NULL)?default_refine_file:refine_file);
    std::vector<size_t> selectedV;
    int vIdx;

    std::cout<<"Read selected vertexes from file...\nSelected Vertexes:"<<std::endl;
    while(fin>>vIdx) {
        std::cout<<vIdx<<" ";
        selectedV.push_back(vIdx);
    }
    fin.close();
    std::cout<<std::endl;

    /* Refinement */
    /* read mesh file */
    std::cout<<"Read mesh from file..."<<std::endl;
    if(!meshReader((input_file == NULL)?default_file:input_file, mesh)){
        /* get vertex - cell pairs */
        mesh.getVI_CI();
        /* detect number of flat angles then print it out */
        mesh.refine(selectedV);
        /* output the processed mesh */
        vtkWriter((output_file == NULL)?"output.vtk":output_file, mesh);
    }else{
        /* fail to read file */
        std::cout << "Fail to read file" << std::endl;
    }

#else

    /* run tests */
    runTest();

#endif

    return 0;
}