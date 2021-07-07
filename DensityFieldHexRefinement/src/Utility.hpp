#ifndef DFHR_UTILITY_HPP
#define DFHR_UTILITY_HPP

#include <eigen3/Eigen/Eigen>

using namespace Eigen;

/*
 * HexVolume()
 * DESCRIPTION: calculate volume of a hex cell using algorithm described in
 *              J. Grandy. Efficient computation of volume of hexahedral cells. 
 *              Lawrence Livermore National Laboratory, October 1997. UCRL-ID-128886.
 * INPUT: sets of all vertexes, a vector of vertex index of a hex cell
 * OUTPUT: none
 * RETURN: volume of hex cell
 */
double HexVolume(const Matrix3Xd &V, const VectorXi &c){
    Vector3d v0 = V.col(c(0)), v1 = V.col(c(1)), v2 = V.col(c(2)), v3 = V.col(c(3)),
             v4 = V.col(c(4)), v5 = V.col(c(5)), v6 = V.col(c(6)), v7 = V.col(c(7));
    Matrix3d M1, M2, M3;
    M1 << v6-v0, v1-v0, v2-v5;
    M2 << v6-v0, v4-v0, v5-v7;
    M3 << v6-v0, v3-v0, v7-v2;
    return (M1.determinant()+M2.determinant()+M3.determinant())/6.;
}

#endif