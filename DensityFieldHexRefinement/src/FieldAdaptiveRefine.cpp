#include "FieldAdaptiveRefine.h"
#include "Utility.hpp"
#include "HexRefine/TrivialRefine.h"
#include "HexPadding/HexPadding.h"
#include "HexEval/HexEval.h"

#define HEX_SIZE 8
#define MAX_ITER_NUM 1

using namespace Eigen;

inline double EvalDensity(const Matrix3Xd &V, const VectorXi &c, const std::function<double(Vector3d)> &DensityField)
{
    Vector3d v0 = V.col(c(0)), v1 = V.col(c(1)), v2 = V.col(c(2)), v3 = V.col(c(3)),
             v4 = V.col(c(4)), v5 = V.col(c(5)), v6 = V.col(c(6)), v7 = V.col(c(7));
    return (DensityField(v0) + DensityField(v1) + DensityField(v2) + DensityField(v3) +
            DensityField(v4) + DensityField(v5) + DensityField(v6) + DensityField(v7)) *
           0.125;
}

inline double EvalDensity(const std::vector<Vector3d> &V, const std::function<double(Vector3d)> &DensityField)
{
    return (DensityField(V.at(0)) + DensityField(V.at(1)) + DensityField(V.at(2)) + DensityField(V.at(3)) +
            DensityField(V.at(4)) + DensityField(V.at(5)) + DensityField(V.at(6)) + DensityField(V.at(7))) *
           0.125;
}

int FieldAdaptiveRefine(Matrix3Xd &V, MatrixXi &C, const std::function<double(Vector3d)> &DensityField, RefineMethod method)
{
    int IterCount = 0;
    std::queue<int> TargetC;
    std::vector<double> HexDensity;
    HexEval::HexEvaluator evaluator;
    HexEval::DensityMetric densityMetric = HexEval::EDGE_LENGTH_METRIC;

    evaluator.EvalDensityField(V, C, densityMetric);
    HexDensity = evaluator.GetDensityField();
    if (MarkTargetHex(V, C, TargetC, DensityField, HexDensity) == -1)
        return -1;

    while (!TargetC.empty() && IterCount++ < MAX_ITER_NUM)
    {
        if (RefineTargetHex(V, C, TargetC, method) == -1)
            return -1;

        evaluator.EvalDensityField(V, C, densityMetric);
        HexDensity = evaluator.GetDensityField();
        if (MarkTargetHex(V, C, TargetC, DensityField, HexDensity) == -1)
            return -1;
    }

    EvalFieldAdaptiveMesh(V, C, DensityField);
    return 0;
}

int MarkTargetHex(const Matrix3Xd &V, const MatrixXi &C, std::queue<int> &TargetC, const std::function<double(Vector3d)> &DensityField, std::vector<double> &HexDensity)
{
    for (int i = 0; i < C.cols(); i++)
    {
        MatrixXi c = C.col(i);
        if (EvalDensity(V, c, DensityField) > HexDensity.at(i))
        {
            TargetC.push(i);
        }
    }
    return 0;
}

int RefineTargetHex(Matrix3Xd &V, MatrixXi &C, std::queue<int> &TargetC, RefineMethod method)
{
    switch (method)
    {
    case TRIVIAL_REFINE:
        if (TrivialRefine(V, C, TargetC) == -1)
            return -1;
        break;
    case PADDING_REFINE:
        if (PaddingRefine(V, C, TargetC) == -1)
            return -1;
        break;
    default:
        break;
    }

    return 0;
}

int TrivialRefine(Matrix3Xd &V, MatrixXi &C, std::queue<int> &TargetC)
{
    HexRefine::Mesh mesh = HexRefine::Mesh();
    // Matrix3Xd RefinedV;
    // MatrixXi RefinedC;
    std::vector<size_t> TargetV;

    for (int i = 0; i < V.cols(); i++)
        mesh.V.push_back(V.col(i));

    for (int i = 0; i < C.cols(); i++)
    {
        HexRefine::Cell c;
        for (int j = 0; j < HEX_SIZE; j++)
        {
            c.push_back(C(j, i));
        }
        mesh.C.push_back(c);
    }

    while (!TargetC.empty())
    {
        for (int i = 0; i < HEX_SIZE; i++)
            TargetV.push_back(C(i, TargetC.front()));
        TargetC.pop();
    }

    mesh.getVI_CI();
    mesh.refine(TargetV);

    C.resize(HEX_SIZE, mesh.C.size());
    V.resize(3, mesh.V.size());

    for (size_t i = 0; i < mesh.V.size(); i++)
        V.col(i) = mesh.V.at(i);

    for (size_t i = 0; i < mesh.C.size(); i++)
    {
        for (size_t j = 0; j < HEX_SIZE; j++)
        {
            C(j, i) = mesh.C.at(i).at(j);
        }
    }

    // V = RefinedV;
    // C = RefinedC;

    return 0;
}

int PaddingRefine(Matrix3Xd &V, MatrixXi &C, std::queue<int> &TargetC)
{
    HexPadding::Mesh mesh = HexPadding::Mesh();
    std::vector<size_t> markedC;

    for (int i = 0; i < V.cols(); i++)
        mesh.V.push_back(V.col(i));

    for (int i = 0; i < C.cols(); i++)
    {
        HexPadding::Cell c;
        for (int j = 0; j < HEX_SIZE; j++)
        {
            c.push_back(C(j, i));
        }
        mesh.C.push_back(c);
    }

    while (!TargetC.empty())
    {
        markedC.push_back(TargetC.front());
        TargetC.pop();
    }

    HexPadding::padding(mesh, markedC, true);

    C.resize(HEX_SIZE, mesh.C.size());
    V.resize(3, mesh.V.size());

    for (size_t i = 0; i < mesh.V.size(); i++)
        V.col(i) = mesh.V.at(i);

    for (size_t i = 0; i < mesh.C.size(); i++)
    {
        for (size_t j = 0; j < HEX_SIZE; j++)
        {
            C(j, i) = mesh.C.at(i).at(j);
        }
    }

    return 0;
}

int EvalFieldAdaptiveMesh(const Matrix3Xd &V, const MatrixXi &C, const std::function<double(Vector3d)> &DensityField)
{
    return 0;
}
