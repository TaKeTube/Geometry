#ifndef REFINE_GLOBAL_HPP
#define REFINE_GLOBAL_HPP

#define MOD(x, n)   (((x%n)+n)%n)
#define HEX_SIZE    8
#define FACE_NUM    6
#define REFINE_EPSILON  1e-5

#include <vector>
#include <unordered_map>
#include "Mesh.h"

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

/* using Möller Trumbore Algorithm to find the intersect point of a plane and a ray */
Vertex intersectRayPlane(const Vertex &v0, const Vertex &v1, const Vertex &v2, 
                         const Vertex &o, const Vector3f &d);

#endif
