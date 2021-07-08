#ifndef FIELD_ADAPTIVE_REFINE_H
#define FIELD_ADAPTIVE_REFINE_H

#include <queue>
#include <vector>
#include <eigen3/Eigen/Eigen>

using namespace Eigen;

inline double EvalDensity(const Matrix3Xd &V, const VectorXi &c, const std::function<double(Vector3d)> &DensityField);

inline double EvalDensity(const std::vector<Vector3d> V, const std::function<double(Vector3d)> &DensityField);

int FieldAdaptiveRefine(Matrix3Xd &V, MatrixXi &C, const std::function<double(Vector3d)> &DensityField);

int MarkTargetHex(const Matrix3Xd &V, const MatrixXi &C, std::queue<int> &TargetC, const std::function<double(Vector3d)> &DensityField);

int RefineTargetHex(Matrix3Xd &V, MatrixXi &C, std::queue<int> &TargetC);

int EvalFieldAdaptiveMesh(const Matrix3Xd &V, const MatrixXi &C, const std::function<double(Vector3d)> &DensityField);

#endif