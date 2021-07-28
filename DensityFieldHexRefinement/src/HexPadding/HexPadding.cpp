#include <set>
#include "HexPadding.h"

#define PADDING_RATIO 0.3
#define SMOOTH_ITERNUM 20

using namespace std;
using namespace Eigen;
using namespace HexPadding;

/*
 * padding()
 * DESCRIPTION: pad the target cells of a given mesh, i.e. add a layer of hex mesh
 * INPUT: hex mesh, indexes of target cells, flag of smoothing
 * OUTPUT: padded hex mesh
 * RETURN: none
 */
void HexPadding::padding(Mesh &m, vector<size_t> markedC, bool smooth, bool markPadded)
{
    Mesh markedSubMesh;

    /* mark marked cells */
    vector<bool> CFlag(m.C.size(), false);
    for (size_t cIdx : markedC)
        CFlag.at(cIdx) = true;

    /* generate mesh from marked cells */
    getSubMesh(m, markedSubMesh, markedC);

    /* get normals and average surface length of the markedMesh */
    markedSubMesh.getFaceInfo();
    markedSubMesh.getSurface();
    markedSubMesh.getSurfaceNormal(m);
    markedSubMesh.getSurfaceAvgLen(m);

    /* get shrinked vertexes of the target submesh */
    unordered_map<size_t, size_t> vMap;
    vector<size_t> &SurfV = markedSubMesh.SurfaceV;
    for (size_t i = 0; i < SurfV.size(); i++)
    {
        size_t vIdx = SurfV.at(i);
        VertInfo &vinfo = markedSubMesh.VinfoMap.at(vIdx);
        /* shrink surface vertexes */
        Vector3d newv = m.V.at(vIdx) - vinfo.surfAvgLen * PADDING_RATIO * vinfo.normal;
        size_t newvIdx = m.addVert(newv);
        /* recording the mapped relation of surface vertexes and its shrinked vertexes */
        vMap[vIdx] = newvIdx;
    }

    /* modify surface cells of the target submesh */
    for (size_t cIdx = 0; cIdx < m.C.size(); cIdx++)
    {
        /* if it is a marked cell */
        if (CFlag.at(cIdx))
        {
            Cell &c = m.C.at(cIdx);
            for (auto &vIdx : c)
            {
                /* if the vertexes has shrinked, i.e. a surface vertexes of submesh  *
                 * change it to its shrinked point                                   */
                if (vMap.find(vIdx) != vMap.end())
                    vIdx = vMap.at(vIdx);
            }
        }
    }

    /* add new layers of cells into the target submesh */
    for (auto &fIdx : markedSubMesh.SurfaceF)
    {
        Cell c(8);
        Face &f = markedSubMesh.F.at(fIdx);

        for (size_t i = 0; i < 4; i++)
        {
            c.at(i) = f.at(i);
            c.at(i + 4) = vMap.at(f.at(i));
        }

        size_t cIdx = m.addCell(c);
        markedC.push_back(cIdx);
        /* mark padded cells if needed */
        if (markPadded) m.PaddedC.push_back(cIdx);
    }

    if (smooth)
    {
        /* smoothing */
        Mesh smoothSubMesh;
        getSubMesh(m, smoothSubMesh, markedC);

        for (int i = 0; i < SMOOTH_ITERNUM; i++)
            volSmoothingSubmeshUsingVerts(m, smoothSubMesh);
    }
}

/*
 * getSubMesh()
 * DESCRIPTION: get submesh of the mesh according to the marked cells
 * INPUT: hex mesh, indexes of target cells
 * OUTPUT: submesh consisting of the marked cells
 * RETURN: none
 */
void HexPadding::getSubMesh(Mesh &mesh, Mesh &subMesh, std::vector<size_t> &markedC)
{
    subMesh.SubV.clear();

    // subMesh.V = mesh.V;

    for (size_t cIdx : markedC)
    {
        Cell &c = mesh.C.at(cIdx);
        subMesh.C.push_back(c);
        for (size_t i = 0; i < 8; i++)
            subMesh.SubV.push_back(c.at(i));
    }

    /* remove repeated sub vertexes of the submesh */
    set<size_t> Vset(subMesh.SubV.begin(), subMesh.SubV.end());
    subMesh.SubV.assign(Vset.begin(), Vset.end());
}

/*
 * volSmoothingUsingCells()
 * DESCRIPTION: smooth internal points according to the center of neighboring cells
 * INPUT: hex mesh
 * OUTPUT: smoothed mesh
 * RETURN: none
 */
void HexPadding::volSmoothingUsingCells(Mesh &mesh)
{
    mesh.getFaceInfo();
    mesh.getSurface();
    mesh.getVertInfo();

    vector<pair<size_t, Vert>> volSmoothMap;
    for (size_t vIdx = 0; vIdx < mesh.V.size(); vIdx++)
    {
        auto &vinfo = mesh.VinfoMap.at(vIdx);
        if (!vinfo.isBoundary)
        {
            /* if it is a internal point, take the average of the centers of its neighbor cells */
            Vert newv = Vector3d::Zero();
            for (size_t cIdx : vinfo.neighborC)
                newv += mesh.getCellCenter(mesh.C.at(cIdx));
            newv /= vinfo.neighborC.size();
            volSmoothMap.push_back(make_pair(vIdx, newv));
        }
    }

    /* modify the geometry of the mesh */
    for (auto &mapPair : volSmoothMap)
        mesh.V.at(mapPair.first) = mapPair.second;
}

/*
 * volSmoothingSubmeshUsingCells()
 * DESCRIPTION: smooth internal points of the submesh according to the center of neighboring cells
 * INPUT: hex mesh, target sub mesh
 * OUTPUT: mesh with submesh region smoothed
 * RETURN: none
 */
void HexPadding::volSmoothingSubmeshUsingCells(Mesh &mesh, Mesh &subMesh)
{
    subMesh.getFaceInfo();
    subMesh.getSurface();
    subMesh.getVertInfo();

    vector<pair<size_t, Vert>> volSmoothMap;
    for (size_t vIdx : subMesh.SubV)
    {
        auto &vinfo = subMesh.VinfoMap.at(vIdx);
        if (!vinfo.isBoundary)
        {
            /* if it is a internal point, take the average of the centers of its neighbor cells */
            Vert newv = Vector3d::Zero();
            for (size_t cIdx : vinfo.neighborC)
                newv += mesh.getCellCenter(subMesh.C.at(cIdx));
            newv /= vinfo.neighborC.size();
            volSmoothMap.push_back(make_pair(vIdx, newv));
        }
    }

    /* modify the geometry of the mesh */
    for (auto &mapPair : volSmoothMap)
        mesh.V.at(mapPair.first) = mapPair.second;
}

/*
 * volSmoothingUsingVerts()
 * DESCRIPTION: smooth internal points according to neighboring vertexes
 * INPUT: hex mesh
 * OUTPUT: smoothed mesh
 * RETURN: none
 */
void HexPadding::volSmoothingUsingVerts(Mesh &mesh)
{
    mesh.getFaceInfo();
    mesh.getSurface();
    mesh.getVertInfo();

    vector<pair<size_t, Vert>> volSmoothMap;
    for (size_t vIdx = 0; vIdx < mesh.V.size(); vIdx++)
    {
        auto &vinfo = mesh.VinfoMap.at(vIdx);
        if (!vinfo.isBoundary)
        {
            /* if it is a internal point, take the average of its neighbor vertexes */
            Vert newv = Vector3d::Zero();
            for (size_t vIdx : vinfo.neighborV)
                newv += mesh.V.at(vIdx);
            newv /= vinfo.neighborV.size();
            volSmoothMap.push_back(make_pair(vIdx, newv));
        }
    }

    /* modify the geometry of the mesh */
    for (auto &mapPair : volSmoothMap)
        mesh.V.at(mapPair.first) = mapPair.second;
}

/*
 * volSmoothingSubmeshUsingEdges()
 * DESCRIPTION: smooth internal points of the submesh according to neighboring vertexes
 * INPUT: hex mesh, target sub mesh
 * OUTPUT: mesh with submesh region smoothed
 * RETURN: none
 */
void HexPadding::volSmoothingSubmeshUsingVerts(Mesh &mesh, Mesh &subMesh)
{
    subMesh.getFaceInfo();
    subMesh.getSurface();
    subMesh.getVertInfo();

    vector<pair<size_t, Vert>> volSmoothMap;
    for (size_t vIdx : subMesh.SubV)
    {
        auto &vinfo = subMesh.VinfoMap.at(vIdx);
        if (!vinfo.isBoundary)
        {
            /* if it is a internal point, take the average of its neighbor vertexes */
            Vert newv = Vector3d::Zero();
            for (size_t vIdx : vinfo.neighborV)
                newv += mesh.V.at(vIdx);
            newv /= vinfo.neighborV.size();
            volSmoothMap.push_back(make_pair(vIdx, newv));
        }
    }

    /* modify the geometry of the mesh */
    for (auto &mapPair : volSmoothMap)
        mesh.V.at(mapPair.first) = mapPair.second;
}