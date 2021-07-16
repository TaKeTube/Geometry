#include "HexEval.h"
#include "Utility.hpp"

using namespace HexEval;
using namespace Eigen;

int HexEvaluator::EvalDensityField(const Matrix3Xd &V, const MatrixXi &C, DensityMetric metric)
{
    if(C.rows() != HEX_SIZE)
        return -1;

    switch (metric)
    {
    case VOLUME_METRIC:
        EvalVolDensity(V, C);
        break;
    case EDGE_LENGTH_METRIC:
        EvalLenDensity(V, C);
        break;
    case RIEMANNIAN_METRIC:
        EvalRiemannianDensity(V, C);
        break;
    default:
        return -1
    }
    return 0;
}

void HexEvaluator::EvalLenDensity(const Matrix3Xd &V, const MatrixXi &C)
{
    EdgeInfoMap.clear();
    for(int cIdx = 0; cIdx < C.size(); cIdx++){
        Edge e;
        for (int i = 0; i < 12; i++){
            e.v1Idx = C(HexEdge[i][0], cIdx);
            e.v2Idx = C(HexEdge[i][0], cIdx);
            EdgeInfoMap[e].len = V.col();
        }
    }
}

std::vector<double> HexEvaluator::GetDensityField()
{
    return DensityField;
}
