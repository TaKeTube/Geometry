#include <cstring>
#include <cstdio>
#include <cassert>

#include "MeshIO.h"
#include "Mesh.h"
#include "global.h"

int main(int argc, char **argv){
    char* input_file = NULL;
    char* output_file = NULL;
    char default_file[] = "../data/bunny.vtk";

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
    /* read mesh file */
    if(!meshReader((input_file == NULL)?default_file:input_file, mesh)){
        /* detect number of flat angles then print it out */
        mesh.detectFlatAngle();
        /* get edge information of the mesh */
        mesh.getE();
        /* remove flat angle by splitting the bad cell */
        mesh.removeFlatAngle();
        /* output the processed mesh */
        vtkWriter((output_file == NULL)?"output.vtk":output_file, mesh);
    }else{
        /* fail to read file */
        std::cout << "Fail to read file" << std::endl;
    }

    return 0;
}