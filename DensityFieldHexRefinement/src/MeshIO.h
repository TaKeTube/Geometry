#ifndef MESH_IO_H
#define MESH_IO_H

#include <eigen3/Eigen/Eigen>
using namespace Eigen;

int meshReader(const char* fname, Matrix3Xd &V, MatrixXi &C);
void vtkReader(const char* fname, Matrix3Xd &V, MatrixXi &C);
void objReader(const char* fname, Matrix3Xd &V, MatrixXi &C);
void vtkWriter(const char* fname, const Matrix3Xd &V, const MatrixXi &C);
void vtkWriter(const char* fname, const Matrix3Xd &V, const MatrixXi &C, std::vector<double> densityField);

#endif
