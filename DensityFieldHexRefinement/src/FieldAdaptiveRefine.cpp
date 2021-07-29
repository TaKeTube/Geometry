#include "FieldAdaptiveRefine.h"
#include "Utility.hpp"
#include "MeshIO.h"
#include "HexRefine/TrivialRefine.h"
#include "HexPadding/HexPadding.h"
#include "HexEval/HexEval.h"

#define HEX_SIZE 8

using namespace Eigen;

/*
 * FieldAdaptiveRefine()
 * DESCRIPTION: refine field according to the given density field
 * INPUT: V - 3xd matrix, each column is a vertex of a mesh
 *        C - 8xd matrix, each column is a cell of a mesh, each cells contains 8 indexes of 8 vertexes in V
 *            following vtk convention
 *        DensityField - scalar function of a 3D vector
 *        method - refine method, having two choices, padding or trivial method
 *        metric - density metric to evaluate the density of a hex cell, having two choices, len or vol metric
 *        iterNum - number of iteration
 *        smooth - whether smooth after each padding - no use for trivial refine
 *        mark - whether output mesh with padded element marked after each padding - no use for trivial refine
 *        eval - whether evaluate the result mesh and output actual field, referece field and difference field
 * OUTPUT: field adaptive refined mesh
 * RETURN: 0 if success, -1 if failed
 */
int FieldAdaptiveRefine(
    Matrix3Xd &V,
    MatrixXi &C,
    const std::function<double(Vector3d)> &DensityField,
    RefineMethod method,
    HexEval::DensityMetric metric,
    int iterNum,
    bool smooth,
    bool mark,
    bool eval)
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
    if (eval)
        EvalFieldAdaptiveMesh(V, C, DensityField);
    return 0;
}

/*
 * MarkTargetHex()
 * DESCRIPTION: mark target hex cells according to reference field
 * INPUT: V - 3xd matrix, each column is a vertex of a mesh
 *        C - 8xd matrix, each column is a cell of a mesh, each cells contains 8 indexes of 8 vertexes in V
 *            following vtk convention
 *        RefDensity - reference density field
 *        HexDensity - density of each element
 * OUTPUT: TargetC  -indexes of target cells
 * RETURN: 0 if success, -1 if failed
 */
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

/*
 * RefineTargetHex()
 * DESCRIPTION: refine target hex cells of the given mesh
 * INPUT: V - 3xd matrix, each column is a vertex of a mesh
 *        C - 8xd matrix, each column is a cell of a mesh, each cells contains 8 indexes of 8 vertexes in V
 *            following vtk convention
 *        TargetC - indexes of target hex cell
 *        method - refine method, having two choices, padding or trivial method
 *        smooth - whether smooth after each padding - no use for trivial refine
 *        mark - whether output mesh with padded element marked after each padding - no use for trivial refine
 * OUTPUT: refined mesh (represented by V, C)
 *         vtk mesh file with padded element marked after each padding if padding method is used and mark flag is active
 * RETURN: 0 if success, -1 if failed
 */
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

/*
 * TrivialRefine()
 * DESCRIPTION: refine target hex cells of the given mesh using trivial method
 * INPUT: V - 3xd matrix, each column is a vertex of a mesh
 *        C - 8xd matrix, each column is a cell of a mesh, each cells contains 8 indexes of 8 vertexes in V
 *            following vtk convention
 *        TargetC - indexes of target hex cell
 * OUTPUT: refined mesh (represented by V, C)
 * RETURN: 0 if success, -1 if failed
 */
int TrivialRefine(Matrix3Xd &V, MatrixXi &C, std::queue<int> &TargetC)
{
    HexRefine::Mesh mesh = HexRefine::Mesh();
    std::vector<size_t> TargetV;

    /* set mesh from C, V */
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

    /* refine */
    mesh.getVI_CI();
    mesh.refine(TargetV);

    /* set C & V from mesh */
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

/*
 * PaddingRefine()
 * DESCRIPTION: refine target hex cells of the given mesh using padding method
 * INPUT: V - 3xd matrix, each column is a vertex of a mesh
 *        C - 8xd matrix, each column is a cell of a mesh, each cells contains 8 indexes of 8 vertexes in V
 *            following vtk convention
 *        TargetC - indexes of target hex cell
 *        smooth - whether smooth after each padding
 *        mark - whether output mesh with padded element marked after each padding
 * OUTPUT: refined mesh (represented by V, C)
 *         vtk mesh file with padded element marked after each padding if mark flag is active
 * RETURN: 0 if success, -1 if failed
 */
int PaddingRefine(Matrix3Xd &V, MatrixXi &C, std::queue<int> &TargetC, bool smooth, bool mark)
{
    static int PadNum = 1;

    HexPadding::Mesh mesh = HexPadding::Mesh();
    std::vector<size_t> markedC;

    /* set mesh from C, V */
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

    /* refine */
    HexPadding::padding(mesh, markedC, smooth, mark);

    /* output mesh with marked elements */
    if (mark)
    {
        std::string outName = std::to_string(PadNum) + "times_paded";
        std::vector<int> PaddedFlag(mesh.C.size(), 0);
        for (size_t cIdx : mesh.PaddedC)
            PaddedFlag.at(cIdx) = 1;
        vtkWriter(outName.c_str(), V, C, PaddedFlag);
    }

    /* set C, V from mesh */
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

/*
 * EvalFieldAdaptiveMesh()
 * DESCRIPTION: evaluate the density of result mesh
 * INPUT: V - 3xd matrix, each column is a vertex of a mesh
 *        C - 8xd matrix, each column is a cell of a mesh, each cells contains 8 indexes of 8 vertexes in V
 *            following vtk convention
 *        DensityField - indexes of target hex cell
 *        metric - density metric to evaluate the density of a hex cell, having two choices, len or vol metric
 * OUTPUT: vtk files of meshes with actual field, reference field and difference field
 * RETURN: 0 if success, -1 if failed
 */
int EvalFieldAdaptiveMesh(const Matrix3Xd &V, const MatrixXi &C, const std::function<double(Vector3d)> &DensityField, HexEval::DensityMetric metric)
{
    HexEval::HexEvaluator evaluator;
    evaluator.setRefDensityField(DensityField);

    if (evaluator.EvalDensityField(V, C, metric) == -1)
        return -1;

    vtkWriter("Field.vtk", V, C, evaluator.GetDensityField());
    vtkWriter("RefField.vtk", V, C, evaluator.GetRefDensityField(V, C));
    vtkWriter("DiffField.vtk", V, C, evaluator.GetDiffDensityField(V, C));
    return 0;
}
