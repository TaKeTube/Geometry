#ifndef HE_UTILITY_HPP
#define HE_UTILITY_HPP

#include <eigen3/Eigen/Eigen>

namespace HexEval
{
    /*
 * HexVolume()
 * DESCRIPTION: calculate volume of a hex cell using algorithm described in
 *              J. Grandy. Efficient computation of volume of hexahedral cells. 
 *              Lawrence Livermore National Laboratory, October 1997. UCRL-ID-128886.
 * INPUT: sets of all vertexes, a vector of vertex index of a hex cell
 * OUTPUT: none
 * RETURN: volume of hex cell
 */
    inline double HexVolume(const Eigen::Matrix3Xd &V, const Eigen::VectorXi &c)
    {
        Eigen::Vector3d v0 = V.col(c(0)), v1 = V.col(c(1)), v2 = V.col(c(2)), v3 = V.col(c(3)),
                        v4 = V.col(c(4)), v5 = V.col(c(5)), v6 = V.col(c(6)), v7 = V.col(c(7));
        Eigen::Matrix3d M1, M2, M3;
        M1 << v6 - v0, v1 - v0, v2 - v5;
        M2 << v6 - v0, v4 - v0, v5 - v7;
        M3 << v6 - v0, v3 - v0, v7 - v2;
        return (M1.determinant() + M2.determinant() + M3.determinant()) / 6.;
    }

    inline double EvalDensity(const Eigen::Matrix3Xd &V, const Eigen::VectorXi &c, const std::function<double(Eigen::Vector3d)> &DensityField)
    {
        Eigen::Vector3d v0 = V.col(c(0)), v1 = V.col(c(1)), v2 = V.col(c(2)), v3 = V.col(c(3)),
                        v4 = V.col(c(4)), v5 = V.col(c(5)), v6 = V.col(c(6)), v7 = V.col(c(7));
        return (DensityField(v0) + DensityField(v1) + DensityField(v2) + DensityField(v3) +
                DensityField(v4) + DensityField(v5) + DensityField(v6) + DensityField(v7)) *
               0.125;
    }
}

#endif