#include "global.hpp"

Vertex intersectRayPlane(const Vertex &v0, const Vertex &v1, const Vertex &v2, 
                         const Vertex &o, const Vector3f &d)
{
    Vector3f E1 = v1 - v0, E2 = v2 - v0, S = o - v0;
    Vector3f S1 = crossProduct(d, E2), S2 = crossProduct(S, E1);
    float inv = 1/dotProduct(S1, E1);
    float t = inv * dotProduct(S2, E2);
    return o + t*d;
}
