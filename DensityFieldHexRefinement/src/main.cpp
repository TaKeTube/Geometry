#include <iostream>

#include "FieldAdaptiveRefine.h"
#include "MeshIO.h"

using namespace Eigen;

int main(int argc, char **argv) {
  char *input_file = NULL;
  char *output_file = NULL;
  char default_file[] = "../data/cad.vtk";

  /*
   *  A standard command:
   *      ./HexRefinement.exe -input "../data/rod.vtk" -output "refined_rod.vtk"
   */
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-input")) {
      i++;
      assert(i < argc);
      input_file = argv[i];
    } else if (!strcmp(argv[i], "-output")) {
      i++;
      assert(i < argc);
      output_file = argv[i];
    } else {
      printf("Error with command line argument %d: '%s'\n", i, argv[i]);
      assert(0);
    }
  }

  Matrix3Xd V;
  MatrixXi C;

  if (!meshReader((input_file == NULL) ? default_file : input_file, V, C)) {
    // FieldAdaptiveRefine(V, C, [](Vector3d v){return
    // 258*exp(-v.squaredNorm()/2)/sqrt(2*M_PI);});
    FieldAdaptiveRefine(V, C, [](Vector3d v) { return 195 * sin(v.y() * 3); });
    vtkWriter((output_file == NULL) ? "output.vtk" : output_file, V, C);
  }
}