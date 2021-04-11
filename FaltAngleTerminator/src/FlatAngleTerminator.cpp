#include <cstring>
#include <cstdio>
#include <cassert>

#include "MeshIO.h"
#include "Mesh.h"
#include "global.h"

int main(int argc, char **argv){
    char* input_file = NULL;
    char* output_file = NULL;
    // char default_file[] = "../data/workpiece.vtk";
    char default_file[] = "../data/spot.obj";

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
    if(!meshReader((input_file == NULL)?default_file:input_file, mesh)){
        std::vector<Cell>& C = mesh.C;
        std::vector<Vertex>& V = mesh.V;
        int badCell[5] = {0};

        for(auto &cell : C){
            int flatNum = 0;
            for(size_t i = 0; i < 4; i++){
                Vertex& v1 = V.at(cell.at(i));
                Vertex& v2 = V.at(cell.at(MOD(i+1, 4)));
                Vertex& v3 = V.at(cell.at(MOD(i-1, 4)));
                if(cosDist(v2-v1, v3-v1) < THRESHOLD)
                    flatNum++;
            }
            badCell[flatNum]++;
        }

        for(int i = 1; i<=4; i++)
            std::cout << "Number of bad Cells with " << i << " flat angles:" << badCell[i] << std::endl;

        mesh.getE();
        mesh.removeFlatAngle();

        vtkWriter("output.vtk" , mesh);
    }else{
        std::cout << "Fail to read file" << std::endl;
    }

    return 0;
}