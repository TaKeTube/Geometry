#include <cstring>
#include <cstdio>
#include <cassert>

#include "MeshIO.h"
#include "Mesh.h"
#include "global.hpp"

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

    Mesh mesh = Mesh();
    std::vector<size_t> selectedV;

    selectedV.push_back(0);

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

    return 0;
}