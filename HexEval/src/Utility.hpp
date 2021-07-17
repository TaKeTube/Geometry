#ifndef DFHR_UTILITY_HPP
#define DFHR_UTILITY_HPP

#include <eigen3/Eigen/Eigen>
#include <vector>
#include <unordered_map>

#define MOD(x, n)   (((x%n)+n)%n)
#define HEX_SIZE    8
#define FACE_NUM    6
#define REFINE_EPSILON  1e-8

/* lookup table of number of 1s in a bit string */
const char bitNumLookup[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2,
    3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3,
    3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3,
    4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4,
    3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5,
    6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4,
    4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5,
    6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5,
    3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3,
    4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6,
    6, 7, 6, 7, 7, 8
};

/* get number of 1s in a bitstring */
inline int getBitNum(unsigned char bitmap){
    return bitNumLookup[(int)bitmap];
}

template <class T>
inline bool isInVec(T &e, std::vector<T> &v){
    return (find(v.begin(), v.end(), e) == v.end()) ? false : true;
}

template <class T, class S>
inline bool isInMap(T &e, std::unordered_map<T, S> &v){
    return (v.find(e) == v.end()) ? false : true;
}

/*
 * HexVolume()
 * DESCRIPTION: calculate volume of a hex cell using algorithm described in
 *              J. Grandy. Efficient computation of volume of hexahedral cells. 
 *              Lawrence Livermore National Laboratory, October 1997. UCRL-ID-128886.
 * INPUT: sets of all vertexes, a vector of vertex index of a hex cell
 * OUTPUT: none
 * RETURN: volume of hex cell
 */
inline double HexVolume(const Eigen::Matrix3Xd &V, const Eigen::VectorXi &c){
    Eigen::Vector3d v0 = V.col(c(0)), v1 = V.col(c(1)), v2 = V.col(c(2)), v3 = V.col(c(3)),
             v4 = V.col(c(4)), v5 = V.col(c(5)), v6 = V.col(c(6)), v7 = V.col(c(7));
    Eigen::Matrix3d M1, M2, M3;
    M1 << v6-v0, v1-v0, v2-v5;
    M2 << v6-v0, v4-v0, v5-v7;
    M3 << v6-v0, v3-v0, v7-v2;
    return (M1.determinant()+M2.determinant()+M3.determinant())/6.;
}

#endif