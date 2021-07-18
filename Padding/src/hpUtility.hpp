#ifndef HEX_PADDING_UTILITY_H
#define HEX_PADDING_UTILITY_H

#include <eigen3/Eigen/Eigen>

/* vertexes order in this table ensures that normal vector of faces points into the cell */
const unsigned int HexFace[6][4] =
{
    { 0,1,2,3 },
    { 4,7,6,5 },
    { 0,4,5,1 },
    { 0,3,7,4 },
    { 3,2,6,7 },
    { 1,5,6,2 },
};

/* return (v1-v0) x (v2-v0) */
inline Eigen::Vector3d getNormal(const Eigen::Vector3d &v0, const Eigen::Vector3d &v1, const Eigen::Vector3d &v2){
    Eigen::Vector3d dv0 = v1 - v0;
    Eigen::Vector3d dv1 = v2 - v0;
    return (dv0.cross(dv1)).normalized();
}

#endif
