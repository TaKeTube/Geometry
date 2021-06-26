#ifndef MESH_IO_H
#define MESH_IO_H

#include <eigen3/Eigen/Eigen>

void vtkWriter(const char* fname , Eigen::Matrix<float, 3, 27> &P, Eigen::Matrix<int, 8, 8> &C);
void vtkWriter(const char* fname , Eigen::Matrix<float, 3, 125> &P, Eigen::Matrix<int, 8, 64> &C);

#endif