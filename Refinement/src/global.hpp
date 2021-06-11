#define MOD(x, n)   (((x%n)+n)%n)
#define HEX_SIZE    8
#define FACE_NUM    6

#include <vector>
#include <unordered_map>
#include "Mesh.h"

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

/* using Möller Trumbore Algorithm */
Vertex intersectRayPlane(Vertex &v0, Vertex &v1, Vertex &v2, Vertex &o, Vector3f &d)
{
    Vector3f E1 = v1 - v0, E2 = v2 - v0, S = o - v0;
    Vector3f S1 = crossProduct(d, E2), S2 = crossProduct(S, E1);
    float inv = 1/dotProduct(S1, E1);
    float t = inv * dotProduct(S2, E2);
    return o + t*d;
}
