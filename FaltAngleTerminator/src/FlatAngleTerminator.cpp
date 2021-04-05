#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "MeshIO.h"
#include "Mesh.h"

int main(int argc, char **argv){
    char* input_file = NULL;
    char* output_file = NULL;
    char default_file[] = "../data/workpiece.vtk";

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i],"-input")) {
            i++; assert (i < argc); 
            input_file = argv[i];
        } else if (!strcmp(argv[i],"-output")) {
            i++; assert (i < argc); 
            output_file = argv[i];
        } else {
            printf ("Error with command line argument %d: '%s'\n",i,argv[i]);
            // assert(0);
        }
    }

    Mesh mesh = Mesh();
    if(input_file)
        vtkReader(input_file, mesh);
    else
        vtkReader(default_file, mesh);

    return 0;
}