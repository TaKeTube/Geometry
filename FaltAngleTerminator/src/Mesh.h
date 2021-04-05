#ifndef MESH_H
#define MESH_H

#include <vector>
#include <hash_map>
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
    ~Vertex();
};

class Mesh
{
public:
    std::vector<Vertex> V;
    std::vector<Cell> C;
    CellType cellType;

    Mesh(const std::vector<Vertex>& v, const std::vector<Cell>& c, const CellType cellType);
    Mesh(const Mesh& mesh);
    Mesh();
    ~Mesh();
};

#endif