#include <set>
#include "HexPadding.h"

#define PADDING_RATIO 0.9

using namespace std;
using namespace Eigen;

void padding(Mesh &m, vector<size_t> &markedC)
{
    Mesh markedSubMesh;

    vector<bool> CFlag(m.C.size(), false);
    for (size_t cIdx : markedC)
        CFlag.at(cIdx) = true;

    /* generate mesh from marked cells */
    getMarkedSubMesh(m, markedSubMesh, markedC);

    /* get normals of vertexes of markedMesh */
    markedSubMesh.getGeometryInfo();
    markedSubMesh.getSurface();
    markedSubMesh.getSurfaceNormal(m);
    markedSubMesh.getSurfaceAvgLen(m);

    unordered_map<size_t, size_t> vMap;
    vector<size_t> &SurfV = markedSubMesh.SurfaceV;
    for (size_t i = 0; i < SurfV.size(); i++)
    {
        size_t vIdx = SurfV.at(i);
        VertInfo &vinfo = markedSubMesh.VinfoMap.at(vIdx);
        Vector3d newv = m.V.at(vIdx) - vinfo.surfAvgLen * PADDING_RATIO * vinfo.normal;
        size_t newvIdx = m.addVert(newv);
        vMap[vIdx] = newvIdx;
    }

    for (size_t cIdx = 0; cIdx < m.C.size(); cIdx++)
    {
        if (CFlag.at(cIdx))
        {
            Cell &c = m.C.at(cIdx);
            for (auto &vIdx : c)
            {
                if (vMap.find(vIdx) != vMap.end())
                    vIdx = vMap.at(vIdx);
            }
        }
    }

    for (auto &fIdx : markedSubMesh.SurfaceF)
    {
        Cell c(8);
        Face &f = markedSubMesh.F.at(fIdx);

        for (size_t i = 0; i < 4; i++)
        {
            c.at(i) = f.at(i);
            c.at(i + 4) = vMap.at(f.at(i));
        }

        m.addCell(c);
    }
}

void getMarkedSubMesh(Mesh &mesh, Mesh &subMesh, std::vector<size_t> &markedC)
{
    subMesh.SubV.clear();

    subMesh.V = mesh.V;

    for (size_t cIdx : markedC)
    {
        Cell &c = mesh.C.at(cIdx);
        subMesh.C.push_back(c);
        for (size_t i = 0; i < 8; i++)
            subMesh.SubV.push_back(c.at(i));
    }

    set<size_t> Vset(subMesh.SubV.begin(), subMesh.SubV.end());
    subMesh.SubV.assign(Vset.begin(), Vset.end());
}