#ifndef FIELD_ADAPTIVE_REFINE_H
#define FIELD_ADAPTIVE_REFINE_H

#include <queue>
#include <eigen3/Eigen/Eigen>

using namespace Eigen;

int FieldAdaptiveRefine(Matrix3Xd &V, MatrixXi &C, std::function<Vector3d> DensityField);

int MarkTargetHex(const Matrix3Xd &V, const MatrixXi &C, std::queue<int> &TargetC, std::function<Vector3d> DensityField);

int RefineTargetHex(Matrix3Xd &V, MatrixXi &C, std::queue<int> &TargetC, Matrix3Xd &RefinedV, MatrixXi &RefinedC);

int EvalFieldAdaptiveMesh(Matrix3Xd &V, MatrixXi &C, std::function<Vector3d> DensityField);

#endif