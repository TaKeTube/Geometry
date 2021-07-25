#ifndef HEX_PADDING_UTILITY_H
#define HEX_PADDING_UTILITY_H

#include <eigen3/Eigen/Eigen>

namespace HexPadding
{
    /* vertexes order in this table ensures that normal vector of faces points into the cell */
    const unsigned int HexFace[6][4] =
        {
            {0, 1, 2, 3},
            {4, 7, 6, 5},
            {0, 4, 5, 1},
            {0, 3, 7, 4},
            {3, 2, 6, 7},
            {1, 5, 6, 2},
    };

    const unsigned int HexVertNeighbor[8][3] =
        {
            {1, 3, 4},
            {0, 2, 5},
            {1, 3, 6},
            {0, 2, 7},
            {0, 5, 7},
            {1, 4, 6},
            {2, 5, 7},
            {3, 4, 6}};

    /*
 * getNormal()
 * DESCRIPTION: get normal vector of a triangle, return (v1-v0) x (v2-v0)
 * INPUT: vertexes of a triangle
 * OUTPUT: (v1-v0) x (v2-v0)
 * RETURN: (v1-v0) x (v2-v0)
 */
    inline Eigen::Vector3d getNormal(const Eigen::Vector3d &v0, const Eigen::Vector3d &v1, const Eigen::Vector3d &v2)
    {
        Eigen::Vector3d dv0 = v1 - v0;
        Eigen::Vector3d dv1 = v2 - v0;
        return (dv0.cross(dv1)).normalized();
    }

    Eigen::Vector3d getClusteredNormal(std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > V)
    {
        size_t vsize = V.size();
        std::vector<char> vflag(vsize, 0);
        bool isClustered = true;
        while(isClustered)
        {
            isClustered = false;
            for(size_t i = 0; i < vsize; i++)
            {
                if(vflag.at(i))
                    continue;
                Eigen::Vector3d &v1 = V.at(i);
                Eigen::Vector3d v = V.at(i);
                int count = 0;
                for(size_t j = i+1; j < vsize; j++)
                {
                    if(vflag.at(j))
                        continue;
                    Eigen::Vector3d &v2 = V.at(j);
                    if((v1-v2).squaredNorm() < 0.3)
                    {
                        v += v2;
                        count++;
                        vflag.at(j) = 1;
                        isClustered = true;
                    }
                }
                if(count)
                    v1 = (v/count).normalized();
            }
        }
        Eigen::Vector3d vavg = Eigen::Vector3d::Zero();
        for(size_t i = 0; i < V.size(); i++)
        {
            if(!vflag.at(i))
                vavg += V.at(i);
        }
        return vavg.normalized();
    }
}

#endif
