#ifndef FIELD_ADAPTIVE_REFINE_H
#define FIELD_ADAPTIVE_REFINE_H

#include <queue>
#include <vector>
#include <eigen3/Eigen/Eigen>

enum RefineMethod
{
    TRIVIAL_REFINE,
    PADDING_REFINE
};

inline double EvalDensity(const Eigen::Matrix3Xd &V, const Eigen::VectorXi &c, const std::function<double(Eigen::Vector3d)> &DensityField);

inline double EvalDensity(const std::vector<Eigen::Vector3d> V, const std::function<double(Eigen::Vector3d)> &DensityField);

int FieldAdaptiveRefine(Eigen::Matrix3Xd &V, Eigen::MatrixXi &C, const std::function<double(Eigen::Vector3d)> &DensityField);

int MarkTargetHex(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C, std::queue<int> &TargetC, const std::function<double(Eigen::Vector3d)> &DensityField, std::vector<double> &HexDensity);

int TrivialMark(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C, std::queue<int> &TargetC, const std::function<double(Eigen::Vector3d)> &DensityField);

int RefineTargetHex(Eigen::Matrix3Xd &V, Eigen::MatrixXi &C, std::queue<int> &TargetC, RefineMethod type);

int TrivialRefine(Eigen::Matrix3Xd &V, Eigen::MatrixXi &C, std::queue<int> &TargetC);

int PaddingRefine(Eigen::Matrix3Xd &V, Eigen::MatrixXi &C, std::queue<int> &TargetC);

int EvalFieldAdaptiveMesh(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C, const std::function<double(Eigen::Vector3d)> &DensityField);

#endif