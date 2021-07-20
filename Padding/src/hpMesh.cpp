#include <tuple>
#include <set>

#include "hpUtility.hpp"
#include "hpMesh.h"

using namespace std;
using namespace Eigen;

Mesh::Mesh(const Mesh& mesh)
: V(mesh.V)
, C(mesh.C)
, cellType(mesh.cellType)
{}
Mesh::Mesh(){}
Mesh::~Mesh(){}

/*
 * addVert()
 * DESCRIPTION: add Vertex into the mesh
 * INPUT: vertex to be added
 * OUTPUT: the index of the added vertex
 * RETURN: the index of the added vertex
 */
size_t Mesh::addVert(Vert &v)
{
    V.push_back(v);
    return V.size() - 1;
}

/*
 * addCell()
 * DESCRIPTION: add Cell into the mesh
 * INPUT: cell to be added
 * OUTPUT: the index of the added cell
 * RETURN: the index of the added cell
 */
size_t Mesh::addCell(Cell &c)
{
    C.push_back(c);
    return C.size() - 1;
}

/*
 * getGeometryInfo()
 * DESCRIPTION: get geometry info of the hex (here it is just face info)
 * INPUT: hex mesh
 * OUTPUT: face info of the mesh
 * RETURN: none
 */
void Mesh::getGeometryInfo()
{
    F.clear();

    vector<Face> totalF(C.size() * 6);
    vector<tuple<Face, size_t>> sortedF(C.size() * 6);

    /* traverse cells */
    for (size_t cIdx = 0; cIdx < C.size(); cIdx++)
    {
        /* for each faces of the cell... */
        for (size_t i = 0; i < 6; i++)
        {
            Face f(4);
            /* for each vertexes of the face... */
            for (size_t j = 0; j < 4; j++)
                f.at(j) = C.at(cIdx).at(HexFace[i][j]);

            size_t fIdx = 6 * cIdx + i;
            totalF.at(fIdx) = f;

            /* sort face fo the sake of removing repeated hex and boundary checking */
            sort(f.begin(), f.end());
            sortedF.at(fIdx) = make_tuple(f, fIdx);
        }
    }

    /* sort F for the sake of removing repeated hex and boundary check */
    sort(sortedF.begin(), sortedF.end());

    /* get F & boundary check */
    F.push_back(totalF.at(get<1>(sortedF.at(0))));
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
    }
}

/*
 * getSurface()
 * DESCRIPTION: get surface of the hex, i.e. surface faces and surface vertexes
 * INPUT: hex mesh
 * OUTPUT: SurfaceF & SurfaceV
 * RETURN: none
 */
void Mesh::getSurface()
{
    SurfaceF.clear();
    SurfaceV.clear();

    /* get surface Vs and Fs */
    for (size_t fIdx = 0; fIdx < F.size(); fIdx++)
    {
        if (FinfoMap.at(fIdx).isBoundary)
        {
            SurfaceF.push_back(fIdx);
            for (size_t i = 0; i < 4; i++)
                SurfaceV.push_back(F.at(fIdx).at(i));
        }
    }

    /* remove repeated surface vertexes */
    set<size_t> Vset(SurfaceV.begin(), SurfaceV.end());
    SurfaceV.assign(Vset.begin(), Vset.end());
}

/*
 * getSurfaceNormal()
 * DESCRIPTION: get surface normal vector for each surface vertexes
 * INPUT: hex mesh
 * OUTPUT: surface normal vector
 * RETURN: none
 */
void Mesh::getSurfaceNormal()
{
    for (size_t fIdx : SurfaceF)
    {
        Face &f = F.at(fIdx);
        size_t v0Idx = f.at(0), v1Idx = f.at(1), v2Idx = f.at(2), v3Idx = f.at(3);

        /* get normal of two triangle of a face */
        Vector3d n0 = -getNormal(V.at(v0Idx), V.at(v1Idx), V.at(v2Idx));
        Vector3d n1 = -getNormal(V.at(v2Idx), V.at(v3Idx), V.at(v0Idx));

        /* old method, accumulate normals of neighbor triangular faces of a vertex then take average */
        // if (VinfoMap.find(v1Idx) == VinfoMap.end())
        //     VinfoMap[v1Idx].normal = n0;
        // else
        //     VinfoMap[v1Idx].normal += n0;

        // if (VinfoMap.find(v2Idx) == VinfoMap.end())
        //     VinfoMap[v2Idx].normal = n0;
        // else
        //     VinfoMap[v2Idx].normal += n0;

        // if (VinfoMap.find(v3Idx) == VinfoMap.end())
        //     VinfoMap[v3Idx].normal = n0;
        // else
        //     VinfoMap[v3Idx].normal += n0;

        // if (VinfoMap.find(v2Idx) == VinfoMap.end())
        //     VinfoMap[v2Idx].normal = n1;
        // else
        //     VinfoMap[v2Idx].normal += n1;

        // if (VinfoMap.find(v3Idx) == VinfoMap.end())
        //     VinfoMap[v3Idx].normal = n1;
        // else
        //     VinfoMap[v3Idx].normal += n1;

        // if (VinfoMap.find(v0Idx) == VinfoMap.end())
        //     VinfoMap[v0Idx].normal = n1;
        // else
        //     VinfoMap[v0Idx].normal += n1;

        /* new method, accumulate normals of neighbor quad faces of a vertex then take average */
        Vector3d n = (n0 + n1).normalized();

        if (VinfoMap.find(v0Idx) == VinfoMap.end())
            VinfoMap[v0Idx].normal = n;
        else
            VinfoMap[v0Idx].normal += n;

        if (VinfoMap.find(v1Idx) == VinfoMap.end())
            VinfoMap[v1Idx].normal = n;
        else
            VinfoMap[v1Idx].normal += n;

        if (VinfoMap.find(v2Idx) == VinfoMap.end())
            VinfoMap[v2Idx].normal = n;
        else
            VinfoMap[v2Idx].normal += n;

        if (VinfoMap.find(v3Idx) == VinfoMap.end())
            VinfoMap[v3Idx].normal = n;
        else
            VinfoMap[v3Idx].normal += n;
    }

    /* normalize the accuamlated normals */
    for (size_t vIdx : SurfaceV)
        VinfoMap[vIdx].normal.normalize();
}

/*
 * getSurfaceNormal()
 * DESCRIPTION: get surface normal vector for each surface vertexes of a submesh (use vertexes of its father mesh)
 * INPUT: hex mesh & its father mesh
 * OUTPUT: surface normal vector
 * RETURN: none
 */
void Mesh::getSurfaceNormal(Mesh &superMesh)
{
    for (size_t fIdx : SurfaceF)
    {
        Face &f = F.at(fIdx);
        size_t v0Idx = f.at(0), v1Idx = f.at(1), v2Idx = f.at(2), v3Idx = f.at(3);

        /* get normal of two triangle of a face */
        Vector3d n0 = -getNormal(superMesh.V.at(v0Idx), superMesh.V.at(v1Idx), superMesh.V.at(v2Idx));
        Vector3d n1 = -getNormal(superMesh.V.at(v2Idx), superMesh.V.at(v3Idx), superMesh.V.at(v0Idx));

        /* old method, accumulate normals of neighbor triangular faces of a vertex then take average */
        // if (VinfoMap.find(v1Idx) == VinfoMap.end())
        //     VinfoMap[v1Idx].normal = n0;
        // else
        //     VinfoMap[v1Idx].normal += n0;

        // if (VinfoMap.find(v2Idx) == VinfoMap.end())
        //     VinfoMap[v2Idx].normal = n0;
        // else
        //     VinfoMap[v2Idx].normal += n0;

        // if (VinfoMap.find(v3Idx) == VinfoMap.end())
        //     VinfoMap[v3Idx].normal = n0;
        // else
        //     VinfoMap[v3Idx].normal += n0;

        // if (VinfoMap.find(v2Idx) == VinfoMap.end())
        //     VinfoMap[v2Idx].normal = n1;
        // else
        //     VinfoMap[v2Idx].normal += n1;

        // if (VinfoMap.find(v3Idx) == VinfoMap.end())
        //     VinfoMap[v3Idx].normal = n1;
        // else
        //     VinfoMap[v3Idx].normal += n1;

        // if (VinfoMap.find(v0Idx) == VinfoMap.end())
        //     VinfoMap[v0Idx].normal = n1;
        // else
        //     VinfoMap[v0Idx].normal += n1;

        /* new method, accumulate normals of neighbor quad faces of a vertex then take average */
        Vector3d n = (n0 + n1).normalized();

        if (VinfoMap.find(v0Idx) == VinfoMap.end())
            VinfoMap[v0Idx].normal = n;
        else
            VinfoMap[v0Idx].normal += n;

        if (VinfoMap.find(v1Idx) == VinfoMap.end())
            VinfoMap[v1Idx].normal = n;
        else
            VinfoMap[v1Idx].normal += n;

        if (VinfoMap.find(v2Idx) == VinfoMap.end())
            VinfoMap[v2Idx].normal = n;
        else
            VinfoMap[v2Idx].normal += n;

        if (VinfoMap.find(v3Idx) == VinfoMap.end())
            VinfoMap[v3Idx].normal = n;
        else
            VinfoMap[v3Idx].normal += n;
    }

    /* normalize the accuamlated normals */
    for (size_t vIdx : SurfaceV)
        VinfoMap[vIdx].normal.normalize();
}

/*
 * getSurfaceAvgLen()
 * DESCRIPTION: get average length of neighbor faces' edges for each surface vertexes
 * INPUT: hex mesh & its father mesh
 * OUTPUT: surface average length and valence of each surface vertexes
 * RETURN: none
 */
void Mesh::getSurfaceAvgLen()
{
    for (size_t fIdx : SurfaceF)
    {
        Face &f = F.at(fIdx);
        size_t v0Idx = f.at(0), v1Idx = f.at(1), v2Idx = f.at(2), v3Idx = f.at(3);

        Vector3d &v0 = V.at(v0Idx);
        Vector3d &v1 = V.at(v1Idx);
        Vector3d &v2 = V.at(v2Idx);
        Vector3d &v3 = V.at(v3Idx);
        double v01len = (v1 - v0).squaredNorm() * 0.5;
        double v12len = (v2 - v1).squaredNorm() * 0.5;
        double v23len = (v3 - v2).squaredNorm() * 0.5;
        double v30len = (v0 - v3).squaredNorm() * 0.5;

        if (VinfoMap.find(v0Idx) == VinfoMap.end())
        {
            VinfoMap[v0Idx].surfAvgLen = v01len + v30len;
            VinfoMap[v0Idx].surfDegree = 1;
        }
        else
        {
            VinfoMap[v0Idx].surfAvgLen += v01len + v30len;
            VinfoMap[v0Idx].surfDegree++;
        }

        if (VinfoMap.find(v1Idx) == VinfoMap.end())
        {
            VinfoMap[v1Idx].surfAvgLen = v01len + v12len;
            VinfoMap[v1Idx].surfDegree = 1;
        }
        else
        {
            VinfoMap[v1Idx].surfAvgLen += v01len + v12len;
            VinfoMap[v1Idx].surfDegree++;
        }

        if (VinfoMap.find(v2Idx) == VinfoMap.end())
        {
            VinfoMap[v2Idx].surfAvgLen = v12len + v23len;
            VinfoMap[v2Idx].surfDegree = 1;
        }
        else
        {
            VinfoMap[v2Idx].surfAvgLen += v12len + v23len;
            VinfoMap[v2Idx].surfDegree++;
        }

        if (VinfoMap.find(v3Idx) == VinfoMap.end())
        {
            VinfoMap[v3Idx].surfAvgLen = v23len + v30len;
            VinfoMap[v3Idx].surfDegree = 1;
        }
        else
        {
            VinfoMap[v3Idx].surfAvgLen += v23len + v30len;
            VinfoMap[v3Idx].surfDegree++;
        }
    }

    /* take average */
    for (size_t vIdx : SurfaceV)
        VinfoMap[vIdx].surfAvgLen /= VinfoMap[vIdx].surfDegree;
}

/*
 * getSurfaceAvgLen()
 * DESCRIPTION: get average length of neighbor faces' edges for each surface vertexes of a submesh (use vertexes of its father mesh)
 * INPUT: hex mesh & its father mesh
 * OUTPUT: surface average length and valence of each surface vertexes
 * RETURN: none
 */
void Mesh::getSurfaceAvgLen(Mesh &superMesh)
{
    for (size_t fIdx : SurfaceF)
    {
        Face &f = F.at(fIdx);
        size_t v0Idx = f.at(0), v1Idx = f.at(1), v2Idx = f.at(2), v3Idx = f.at(3);

        Vector3d &v0 = superMesh.V.at(v0Idx);
        Vector3d &v1 = superMesh.V.at(v1Idx);
        Vector3d &v2 = superMesh.V.at(v2Idx);
        Vector3d &v3 = superMesh.V.at(v3Idx);
        double v01len = (v1 - v0).squaredNorm() * 0.5;
        double v12len = (v2 - v1).squaredNorm() * 0.5;
        double v23len = (v3 - v2).squaredNorm() * 0.5;
        double v30len = (v0 - v3).squaredNorm() * 0.5;

        if (VinfoMap.find(v0Idx) == VinfoMap.end())
        {
            VinfoMap[v0Idx].surfAvgLen = v01len + v30len;
            VinfoMap[v0Idx].surfDegree = 1;
        }
        else
        {
            VinfoMap[v0Idx].surfAvgLen += v01len + v30len;
            VinfoMap[v0Idx].surfDegree++;
        }

        if (VinfoMap.find(v1Idx) == VinfoMap.end())
        {
            VinfoMap[v1Idx].surfAvgLen = v01len + v12len;
            VinfoMap[v1Idx].surfDegree = 1;
        }
        else
        {
            VinfoMap[v1Idx].surfAvgLen += v01len + v12len;
            VinfoMap[v1Idx].surfDegree++;
        }

        if (VinfoMap.find(v2Idx) == VinfoMap.end())
        {
            VinfoMap[v2Idx].surfAvgLen = v12len + v23len;
            VinfoMap[v2Idx].surfDegree = 1;
        }
        else
        {
            VinfoMap[v2Idx].surfAvgLen += v12len + v23len;
            VinfoMap[v2Idx].surfDegree++;
        }

        if (VinfoMap.find(v3Idx) == VinfoMap.end())
        {
            VinfoMap[v3Idx].surfAvgLen = v23len + v30len;
            VinfoMap[v3Idx].surfDegree = 1;
        }
        else
        {
            VinfoMap[v3Idx].surfAvgLen += v23len + v30len;
            VinfoMap[v3Idx].surfDegree++;
        }
    }

    /* take average */
    for (size_t vIdx : SurfaceV)
        VinfoMap[vIdx].surfAvgLen /= VinfoMap[vIdx].surfDegree;
}