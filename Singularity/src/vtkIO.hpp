#ifndef VTK_IO_H
#define VTK_IO_H

#include "Singularity.hpp"

/* write singular lines into vtk file */
void vtkSingularitiesWriter(OpenVolumeMesh::GeometricPolyhedralMeshV3f &mMesh,
                            std::vector<Edge> &singularity,
                            const char *fname);

#endif