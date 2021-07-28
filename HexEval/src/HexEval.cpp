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

/*
 * EvalDensityField()
 * DESCRIPTION: evaluate density field of a hex mesh. There are three types of metric
 *              len - reciprocal of the average of edges of a hex cell
 *              vol - reciprocal of the volume of a hex cell
 *              anisotropic - average of edge metrics described in 
 *                            Automated refinement of conformal quadrilateral and hexahedral meshes
 *                                                   - Tchon KF, Dompierre J, Camarero R*  
 *                            of a hex cell
 *                            PS: By using the average of edge metrics, the metric is actually not anisotropic
 * INPUT: V - 3xd matrix, each column is a vertex of a mesh
 *        C - 8xd matrix, each column is a cell of a mesh, each cells contains 8 indexes of 8 vertexes in V
 *            following vtk convention
 *        metric - density metric
 * OUTPUT: densityfield in HexEvaluator
 * RETURN: 0 if success, 01 if failed
 */
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

/*
 * EvalVolDensity()
 * DESCRIPTION: evaluate density of a mesh using vol metric
 * INPUT: V - 3xd matrix, each column is a vertex of a mesh
 *        C - 8xd matrix, each column is a cell of a mesh, each cells contains 8 indexes of 8 vertexes in V
 *            following vtk convention
 * OUTPUT: densityfield in HexEvaluator
 * RETURN: none
 */
void HexEvaluator::EvalVolDensity(const Matrix3Xd &V, const MatrixXi &C)
{
    DensityField.clear();

    for (int cIdx = 0; cIdx < C.cols(); cIdx++)
        DensityField.push_back(1 / HexVolume(V, C.col(cIdx)));
}

/*
 * EvalLenDensity()
 * DESCRIPTION: evaluate density of a mesh using len metric
 * INPUT: V - 3xd matrix, each column is a vertex of a mesh
 *        C - 8xd matrix, each column is a cell of a mesh, each cells contains 8 indexes of 8 vertexes in V
 *            following vtk convention
 * OUTPUT: densityfield in HexEvaluator
 * RETURN: none
 */
void HexEvaluator::EvalLenDensity(const Matrix3Xd &V, const MatrixXi &C)
{
    double edgeLen = 0;

    DensityField.clear();
    EdgeLenMap.clear();

    /* record length of every edges */
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

    /* calculate reciprocal of average length of edges of a hex cell */
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

/*
 * EvalAnisotropicDensity()
 * DESCRIPTION: evaluate density of a mesh using anisotropic metric
 * INPUT: V - 3xd matrix, each column is a vertex of a mesh
 *        C - 8xd matrix, each column is a cell of a mesh, each cells contains 8 indexes of 8 vertexes in V
 *            following vtk convention
 * OUTPUT: densityfield in HexEvaluator
 * RETURN: none
 */
void HexEvaluator::EvalAnisotropicDensity(const Matrix3Xd &V, const MatrixXi &C)
{
    double edgeLen = 0;

    DensityField.clear();
    EdgeAnisotropicMetricMap.clear();

    /* record anisotropic metric of every edges */
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

    /* calculate reciprocal of average metric of edges of a hex cell */
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

/*
 * GetRefDensityField()
 * DESCRIPTION: evaluate density of each cell of a mesh using reference field
 *              for each cell, evaluate the average of values of the reference field at 8 vertexes
 * INPUT: V - 3xd matrix, each column is a vertex of a mesh
 *        C - 8xd matrix, each column is a cell of a mesh, each cells contains 8 indexes of 8 vertexes in V
 *            following vtk convention
 * OUTPUT: reference density field in HexEvaluator
 * RETURN: none
 */
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

/*
 * GetDiffDensityField()
 * DESCRIPTION: evaluate difference of reference field and actual field
 * INPUT: V - 3xd matrix, each column is a vertex of a mesh
 *        C - 8xd matrix, each column is a cell of a mesh, each cells contains 8 indexes of 8 vertexes in V
 *            following vtk convention
 * OUTPUT: difference density field in HexEvaluator
 * RETURN: none
 */
std::vector<double> HexEvaluator::GetDiffDensityField(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C)
{
    std::vector<double> diffField;
    for (int cIdx = 0; cIdx < C.cols(); cIdx++)
    {
        MatrixXi c = C.col(cIdx);
        diffField.push_back(DensityField.at(cIdx)-EvalDensity(V, c, RefDensityField));
    }
    return diffField;
}

/*
 * setRefDensityField()
 * DESCRIPTION: set reference density field
 * INPUT: reference density field
 * OUTPUT: reference density field in HexEvaluator
 * RETURN: none
 */
void HexEvaluator::setRefDensityField(const std::function<double(Vector3d)> &DensityField)
{
    RefDensityField = DensityField;
}

/*
 * setAnisotropicDensityField()
 * DESCRIPTION: set anisotropic reference density field
 * INPUT: anisotropic reference density field
 * OUTPUT: anisotropic reference density field in HexEvaluator
 * RETURN: none
 */
void HexEvaluator::setAnisotropicDensityField(std::function<Eigen::Matrix3d(Eigen::Vector3d)> &DensityField){
    AnisotropicDensityField = DensityField;
}

/*
 * GetDensityField()
 * DESCRIPTION: get evaluated density field
 * INPUT: none
 * OUTPUT: evaluated density field in HexEvaluator
 * RETURN: none
 */
std::vector<double> HexEvaluator::GetDensityField()
{
    return DensityField;
}
