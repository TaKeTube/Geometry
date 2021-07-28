#include "FieldAdaptiveRefine.h"
#include "Utility.hpp"
#include "MeshIO.h"
#include "HexRefine/TrivialRefine.h"
#include "HexPadding/HexPadding.h"
#include "HexEval/HexEval.h"

#define HEX_SIZE 8

using namespace Eigen;

int FieldAdaptiveRefine(
                        Matrix3Xd &V,
                        MatrixXi &C,
                        const std::function<double(Vector3d)> &DensityField, 
                        RefineMethod method,
                        HexEval::DensityMetric metric,
                        int iterNum,
                        bool smooth,
                        bool mark,
                        bool eval
                        )
{
    int IterCount = 0;
    std::queue<int> TargetC;
    std::vector<double> HexDensity;
    std::vector<double> RefDensity;
    HexEval::HexEvaluator evaluator;

    while ((!TargetC.empty()) && (IterCount++ < iterNum))
    {
        /* evaluate hex density */
        evaluator.EvalDensityField(V, C, metric);
        evaluator.setRefDensityField(DensityField);
        HexDensity = evaluator.GetDensityField();
        RefDensity = evaluator.GetRefDensityField(V, C);

        /* according to hex density and reference field, mark target hex cells */
        if (MarkTargetHex(V, C, TargetC, RefDensity, HexDensity) == -1)
            return -1;

        /* refine according to target hex cells */
        if (RefineTargetHex(V, C, TargetC, method, smooth, mark) == -1)
            return -1;
    }

    /* evaluate result hex */
    if(eval)
        EvalFieldAdaptiveMesh(V, C, DensityField);
    return 0;
}

int MarkTargetHex(const Matrix3Xd &V, const MatrixXi &C, std::queue<int> &TargetC, std::vector<double> &RefDensity, std::vector<double> &HexDensity)
{
    for (int i = 0; i < C.cols(); i++)
    {
        MatrixXi c = C.col(i);
        if (RefDensity.at(i) > HexDensity.at(i))
        {
            TargetC.push(i);
        }
    }
    return 0;
}

int RefineTargetHex(Matrix3Xd &V, MatrixXi &C, std::queue<int> &TargetC, RefineMethod method, bool smooth, bool mark)
{
    switch (method)
    {
    case TRIVIAL_REFINE:
        if (TrivialRefine(V, C, TargetC) == -1)
            return -1;
        break;
    case PADDING_REFINE:
        if (PaddingRefine(V, C, TargetC, smooth, mark) == -1)
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

    return 0;
}

int PaddingRefine(Matrix3Xd &V, MatrixXi &C, std::queue<int> &TargetC, bool smooth, bool mark)
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

    HexPadding::padding(mesh, markedC, smooth, mark);

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

int EvalFieldAdaptiveMesh(const Matrix3Xd &V, const MatrixXi &C, const std::function<double(Vector3d)> &DensityField, HexEval::DensityMetric metric)
{
    HexEval::HexEvaluator evaluator;
    evaluator.setRefDensityField(DensityField);
    if (metric == HexEval::ANISOTROPIC_METRIC)
    {
        std::function<Eigen::Matrix3d(Eigen::Vector3d)> anisotropicField = [](Vector3d v)
        { return Eigen::MatrixXd::Identity(3, 3); };
        evaluator.setAnisotropicDensityField(anisotropicField);
    }
    if (evaluator.EvalDensityField(V, C, metric) == -1)
        return -1;
    vtkWriter("Field.vtk",      V, C, evaluator.GetDensityField());
    vtkWriter("RefField.vtk",   V, C, evaluator.GetRefDensityField(V, C));
    vtkWriter("DiffField.vtk",  V, C, evaluator.GetDiffDensityField(V, C));
    return 0;
}
