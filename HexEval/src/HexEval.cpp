#include <iostream>
#include "HexEval.h"
#include "heIntegral.h"
#include "heUtility.hpp"

#define HEX_SIZE    8

using namespace HexEval;
using namespace Eigen;

/* constructior & destructior for class Edge */
Edge::Edge() {}
Edge::Edge(size_t v1, size_t v2) : v1Idx(v1), v2Idx(v2) {}


int HexEvaluator::EvalDensityField(const Matrix3Xd &V, const MatrixXi &C, DensityMetric metric)
{
    if (C.rows() != HEX_SIZE)
        return -1;

    switch (metric)
    {
    case VOLUME_METRIC:
        EvalVolDensity(V, C);
        break;
    case EDGE_LENGTH_METRIC:
        EvalLenDensity(V, C);
        break;
    case ANISOTROPIC_METRIC:
        if (AnisotropicDensityField != nullptr)
        {
            EvalAnisotropicDensity(V, C);
        }
        else
        {
            std::cout << "HexEval::HexEvaluator::EvalDensityField Error:" << std::endl;
            std::cout << "    Isotropic density field is not specified. Please use setRefDensityField to set reference density field." << std::endl;
            return -1;
        }
        break;
    default:
        std::cout << "HexEval::HexEvaluator::EvalDensityField Error:" << std::endl;
        std::cout << "    No such evaluation method." << std::endl;
        return -1;
    }
    return 0;
}

void HexEvaluator::EvalVolDensity(const Matrix3Xd &V, const MatrixXi &C)
{
    DensityField.clear();

    for (int cIdx = 0; cIdx < C.cols(); cIdx++)
        DensityField.push_back(1 / HexVolume(V, C.col(cIdx)));
}

void HexEvaluator::EvalLenDensity(const Matrix3Xd &V, const MatrixXi &C)
{
    double edgeLen = 0;

    DensityField.clear();
    EdgeLenMap.clear();

    for (int cIdx = 0; cIdx < C.cols(); cIdx++)
    {
        Edge e;
        for (int i = 0; i < 12; i++)
        {
            e.v1Idx = C(HexEdge[i][0], cIdx);
            e.v2Idx = C(HexEdge[i][1], cIdx);
            if (EdgeLenMap.find(e) == EdgeLenMap.end())
                EdgeLenMap[e] = (V.col(e.v1Idx) - V.col(e.v2Idx)).squaredNorm();
        }
    }

    for (int cIdx = 0; cIdx < C.cols(); cIdx++)
    {
        Edge e;
        edgeLen = 0;
        for (int i = 0; i < 12; i++)
        {
            e.v1Idx = C(HexEdge[i][0], cIdx);
            e.v2Idx = C(HexEdge[i][1], cIdx);
            edgeLen += EdgeLenMap[e];
        }
        DensityField.push_back(12 / edgeLen);
    }
}

void HexEvaluator::EvalAnisotropicDensity(const Matrix3Xd &V, const MatrixXi &C)
{
    double edgeLen = 0;

    DensityField.clear();
    EdgeAnisotropicMetricMap.clear();

    for (int cIdx = 0; cIdx < C.cols(); cIdx++)
    {
        Edge e;
        for (int i = 0; i < 12; i++)
        {
            e.v1Idx = C(HexEdge[i][0], cIdx);
            e.v2Idx = C(HexEdge[i][1], cIdx);
            if (EdgeAnisotropicMetricMap.find(e) == EdgeAnisotropicMetricMap.end())
            {
                Vector3d v1 = V.col(e.v1Idx);
                Vector3d v2 = V.col(e.v2Idx);
                Vector3d dV = v2 - v1;
                std::function<double(double)> metric = [&v1, &dV, this](double t)
                { return sqrt((dV.transpose() * AnisotropicDensityField(v1 + dV * 5) * dV)); };
                EdgeAnisotropicMetricMap[e] = integrateSimpson(0, 1, metric, 1000);
            }
        }
    }

    for (int cIdx = 0; cIdx < C.cols(); cIdx++)
    {
        Edge e;
        edgeLen = 0;
        for (int i = 0; i < 12; i++)
        {
            e.v1Idx = C(HexEdge[i][0], cIdx);
            e.v2Idx = C(HexEdge[i][1], cIdx);
            edgeLen += EdgeAnisotropicMetricMap[e];
        }
        DensityField.push_back(12 / edgeLen);
    }
}

std::vector<double> HexEvaluator::GetRefDensityField(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C)
{
    std::vector<double> refField;
    for (int cIdx = 0; cIdx < C.cols(); cIdx++)
    {
        MatrixXi c = C.col(cIdx);
        refField.push_back(EvalDensity(V, c, RefDensityField));
    }
    return refField;
}

std::vector<double> HexEvaluator::GetDiffDensityField(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C)
{
    std::vector<double> diffField;
    for (int cIdx = 0; cIdx < C.cols(); cIdx++)
    {
        MatrixXi c = C.col(cIdx);
        diffField.push_back(EvalDensity(V, c, RefDensityField)-DensityField.at(cIdx));
    }
    return diffField;
}

void HexEvaluator::setRefDensityField(const std::function<double(Vector3d)> &DensityField)
{
    RefDensityField = DensityField;
}

void HexEvaluator::setAnisotropicDensityField(std::function<Eigen::Matrix3d(Eigen::Vector3d)> &DensityField){
    AnisotropicDensityField = DensityField;
}

std::vector<double> HexEvaluator::GetDensityField()
{
    return DensityField;
}
