#include <cstring>
#include <cstdio>
#include <cassert>
#include <string>
#include <sstream>

#include "MeshIO.h"
#include "Mesh.h"
#include "global.hpp"

#define TEST 1
// #define TEST_1_VERT
#define TEST_2_VERT

int main(int argc, char **argv){
    char* input_file = NULL;
    char* output_file = NULL;
    char default_file[] = "../data/cube.vtk";

    /* 
     *  A standard command: 
     *      ./FlatAngleTerminator.exe -input "../data/cube.obj" -output "processed_cube.vtk"
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

#if !TEST

    Mesh mesh = Mesh();
    std::vector<size_t> selectedV;

    selectedV.push_back(1);

    /* read mesh file */
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

    char test_file[] = "../data/cube.vtk";

    #ifdef TEST_1_VERT
    /* test 1 vertex selected */
    for(int i = 0; i < HEX_SIZE; i++){
        std::string idx;
        Mesh mesh = Mesh();

        /* select target vertex */
        std::vector<size_t> selectedV;
        selectedV.push_back(i);
        
        /* read mesh */
        meshReader(test_file, mesh);
        
        /* get vertex - cell pairs */
        mesh.getVI_CI();
        
        /* detect number of flat angles then print it out */
        mesh.refine(selectedV);
        
        /* output the processed mesh */
        idx = '0'+(char)i;
        vtkWriter((output_file == NULL)?(std::string("v")+idx+std::string(".vtk")).c_str():output_file, mesh);
    }
    #endif

    #ifdef TEST_2_VERT
    /* test 2 vertex selected */
    for(int i = 0; i < HEX_SIZE; i++){
        for(int j = i+1; j < HEX_SIZE; j++){
            Mesh mesh = Mesh();
            std::string iIdx, jIdx;

            /* select 2 target vertexes */
            std::vector<size_t> selectedV;
            selectedV.push_back(i);
            selectedV.push_back(j);
            
            /* read mesh */
            meshReader(test_file, mesh);
            
            /* get vertex - cell pairs */
            mesh.getVI_CI();
            
            /* detect number of flat angles then print it out */
            mesh.refine(selectedV);
            
            /* output the processed mesh */
            iIdx = '0'+(char)i;
            jIdx = '0'+(char)j;
            vtkWriter((output_file == NULL)?(std::string("v")+iIdx+jIdx+std::string(".vtk")).c_str():output_file, mesh);
        }
    }
    #endif

#endif

    return 0;
}