#ifndef FIELD_ADAPTIVE_REFINE_H
#define FIELD_ADAPTIVE_REFINE_H

#include <queue>
#include <vector>
#include <eigen3/Eigen/Eigen>

#include "HexEval/HexEval.h"

enum RefineMethod
{
    TRIVIAL_REFINE,
    PADDING_REFINE
};

inline double EvalDensity(const Eigen::Matrix3Xd &V, const Eigen::VectorXi &c, const std::function<double(Eigen::Vector3d)> &DensityField);

inline double EvalDensity(const std::vector<Eigen::Vector3d> V, const std::function<double(Eigen::Vector3d)> &DensityField);

int FieldAdaptiveRefine(Eigen::Matrix3Xd &V, Eigen::MatrixXi &C, const std::function<double(Eigen::Vector3d)> &DensityField, RefineMethod method, HexEval::DensityMetric, int iterNum, bool smooth, bool mark, bool eval);

int MarkTargetHex(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C, std::queue<int> &TargetC, std::vector<double> &RefDensity, std::vector<double> &HexDensity);

int TrivialMark(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C, std::queue<int> &TargetC, const std::function<double(Eigen::Vector3d)> &DensityField);

int RefineTargetHex(Eigen::Matrix3Xd &V, Eigen::MatrixXi &C, std::queue<int> &TargetC, RefineMethod method, bool smooth, bool mark);

int TrivialRefine(Eigen::Matrix3Xd &V, Eigen::MatrixXi &C, std::queue<int> &TargetC);

int PaddingRefine(Eigen::Matrix3Xd &V, Eigen::MatrixXi &C, std::queue<int> &TargetC, bool smooth, bool mark);

int EvalFieldAdaptiveMesh(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C, const std::function<double(Eigen::Vector3d)> &DensityField, HexEval::DensityMetric metric);

#endif