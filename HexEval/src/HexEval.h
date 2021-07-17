#ifndef HEX_EVAL_H
#define HEX_EVAL_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <eigen3/Eigen/Eigen>

namespace HexEval
{
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

namespace HexEval
{
    enum DensityMetric
    {
        VOLUME_METRIC,
        EDGE_LENGTH_METRIC,
        ANISOTROPIC_METRIC
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

    class HexEvaluator
    {
    public:
        // HexEvaluator() : RefDensityField(nullptr), AnisotropicDensityField(nullptr) {};
        // ~HexEvaluator();

        int EvalDensityField(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C, DensityMetric metric);
        std::vector<double> GetDensityField();

        void setRefDensityField(const std::function<double(Eigen::Vector3d)> &DensityField);
        void setAnisotropicDensityField(std::function<Eigen::Matrix3d(Eigen::Vector3d)> &DensityField);

    private:
        std::vector<double> DensityField;
        std::unordered_map<Edge, double> EdgeLenMap;
        std::unordered_map<Edge, double> EdgeAnisotropicMetricMap;

        std::function<double(Eigen::Vector3d)> RefDensityField;
        std::function<Eigen::Matrix3d(Eigen::Vector3d)> AnisotropicDensityField;

        void EvalVolDensity(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C);
        void EvalLenDensity(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C);
        void EvalAnisotropicDensity(const Eigen::Matrix3Xd &V, const Eigen::MatrixXi &C);
    };
}

#endif