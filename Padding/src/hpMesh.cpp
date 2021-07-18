#include <tuple>

#include "hpUtility.hpp"
#include "hpMesh.h"

using namespace std;

void Mesh::getGeometryInfo()
{
    F.clear();

    vector<Face> totalF(C.size() * 6);
    vector<tuple<Face, size_t, size_t, size_t>> sortedF(C.size() * 6);

    /* traverse cells */
    for (size_t cIdx = 0; cIdx < C.size(); cIdx++)
    {
        /* for each faces of the cell... */
        for (size_t i = 0; i < 6; i++)
        {
            /* for each vertexes of the face... */
            for (size_t j = 0; j < 4; j++)
            {
                Face f(4);
                size_t fIdx = 6 * cIdx + i;

                f.at(j) = C.at(cIdx).at(HexFace[i][j]);
                totalF.at(fIdx) = f;

                /* sort face */
                sort(f.begin(), f.end());
                sortedF.at(fIdx) = make_tuple(f, fIdx, i, j);
            }
            CinfoMap[cIdx].F.resize(6);
        }

        /* sort F for the sake of boundary check */
        sort(sortedF.begin(), sortedF.end());

        /* get F & boundary check */
        F.push_back(totalF.at(get<1>(sortedF.at(0))));
        CinfoMap[get<1>(sortedF.at(0))].F[get<2>(sortedF.at(0))] = 0;
        FinfoMap[F.size() - 1].isBoundary = true;

        for (size_t i = 1; i < sortedF.size(); i++)
        {
            if (get<0>(sortedF.at(i)) != get<0>(sortedF.at(i - 1)))
            {
                /* a new different face, add it into F */
                F.push_back(totalF.at(get<1>(sortedF.at(i))));
                FinfoMap[F.size() - 1].isBoundary = true;
            }
            else
            {
                /* repeated faces, which means it is not a boundary face */
                FinfoMap[F.size() - 1].isBoundary = false;
            }

            CinfoMap[get<1>(sortedF.at(i))].F[get<2>(sortedF.at(i))] = F.size() - 1;
        }
    }
}

void Mesh::getSurfaceNormal()
{

}