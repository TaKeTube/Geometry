#ifndef HEX_EVAL_H
#define HEX_EVAL_H

#include <vector>
#include <eigen3/Eigen/Eigen>

namespace HexEval
{
    enum DensityMetric
    {
        VOLUME_METRIC,
        EDGE_LENGTH_METRIC,
        RIEMANNIAN_METRIC
    };

    const unsigned int HexEdge[12][2] =
        {
            {0, 1},
            {1, 2},
            {2, 3},
            {3, 0},
            {4, 5},
            {5, 6},
            {6, 7},
            {7, 4},
            {0, 4},
            {1, 5},
            {2, 6},
            {3, 7},
    };

    /* Edge related */
    class Edge
    {
    public:
        size_t v1Idx, v2Idx;

        Edge();
        Edge(size_t v1, size_t v2);

        bool operator==(const Edge &e) const
        {
            return ((v1Idx == e.v1Idx) && (v2Idx == e.v2Idx)) || ((v1Idx == e.v2Idx) && (v2Idx == e.v1Idx));
        }
    };

    struct EdgeInfo
    {
        std::vector<size_t> cellIdxVec;
        double len;
    };

    class HexEvaluator
    {
    public:
        int EvalDensityField(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C, DensityMetric metric);
        std::vector<double> HexEvaluator::GetDensityField();

    private:
        std::vector<double> DensityField;
        std::unordered_map<Edge, EdgeInfo> EdgeInfoMap;

        void EvalVolDensity(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C);
        void EvalLenDensity(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C);
        void EvalRiemannianDensity(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C);
    };
}

/* hash function for edge unordered_map */
namespace std
{
    template <>
    struct hash<HexEval::Edge>
    {
        size_t operator()(const HexEval::Edge &e) const noexcept
        {
            return hash<decltype(e.v1Idx)>()(e.v1Idx) +
                   hash<decltype(e.v2Idx)>()(e.v2Idx);
        }
    };
}

#endif