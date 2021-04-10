#ifndef MESH_H
#define MESH_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include "Vector.hpp"

enum CellType {TRIANGLE, QUAD, TETRAHEDRA, HEXAHEDRA, POLYGON};

typedef std::vector<int> Cell;

class Vertex : public Vector3f
{
public:
    std::vector<int> Nbr;

    Vertex();
    Vertex(float xx);
    Vertex(float xx, float yy, float zz);
    Vertex(float xx, float yy, float zz, std::vector<int>& neighbor);
    Vertex(Vector3f v);
    ~Vertex();
};

class Edge
{
public:
    size_t  v1Idx, v2Idx;

    Edge();
    Edge(size_t v1, size_t v2);

    bool operator == (const Edge& e) const{
        return (v1Idx == e.v1Idx && v2Idx == e.v2Idx) || (v1Idx == e.v2Idx && v2Idx == e.v1Idx);
    }
};

class EdgeInfo
{
public:
    bool isSplit;
    std::vector<size_t> cellIdxVec;

    EdgeInfo();
    EdgeInfo(size_t cellIdx);
};

namespace std
{
    template<>
    struct hash<Edge>
    {
        size_t operator() (const Edge& e) const noexcept
        {
            return  hash<decltype(e.v1Idx)>()(e.v1Idx) +
                    hash<decltype(e.v2Idx)>()(e.v2Idx);
        }
    };
}

class Mesh
{
public:
    std::vector<Vertex> V;
    std::vector<Cell> C;
    std::unordered_map<Edge, EdgeInfo> E;
    CellType cellType;

    Mesh(const std::vector<Vertex>& v, const std::vector<Cell>& c, const CellType cellType);
    Mesh(const Mesh& mesh);
    Mesh();
    ~Mesh();

    void getE();
    size_t addVertex(Vertex v);
    void addQuadCell(size_t v1, size_t v2, size_t v3, size_t v4);
    void deleteCell(size_t idx);
    int removeFlatAngle();
};

#endif