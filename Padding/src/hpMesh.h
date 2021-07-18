#ifndef HEX_PADDING_MESH_H
#define HEX_PADDING_MESH_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <eigen3/Eigen/Eigen>

/* Cell related */
enum MeshType {TRIANGLE, QUAD, TETRAHEDRA, HEXAHEDRA, POLYGON};

struct Cell
{
    std::vector<size_t> V; // vertexes
    std::vector<size_t> E; // edges
    std::vector<size_t> F; // faces
    bool isBoundary;
};

struct Face
{
    std::vector<size_t> V; // vertexes
    std::vector<size_t> E; // edges
    std::vector<size_t> neighborC;    // neighbor cells
    bool isBoundary;
};

struct Edge
{
    std::vector<size_t> V; // vertexes
    std::vector<size_t> neighborF;    // neighbor faces
    std::vector<size_t> neighborC;    // neighbor cells
    bool isBoundary;
};

struct Vert
{
    Eigen::Vector3d v; // vertexes
    std::vector<size_t> neighborV;   // neighbor vertexes
    std::vector<size_t> neighborE;   // neighbor edges
    std::vector<size_t> neighborF;   // neighbor faces
    std::vector<size_t> neighborC;   // neighbor cells
    bool isBoundary;

    Eigen::Vector3d normal; // normal vector
};

/* Mesh related */
class Mesh
{
public:
    std::vector<Vert> V;
    std::vector<Cell> C;
    std::vector<Edge> E;
    std::vector<Face> F;
    MeshType cellType;

    Mesh();
    Mesh(const Mesh& mesh);
    // Mesh(const std::vector<Vertex>& v, const std::vector<Cell>& c, const MeshType meshType);
    ~Mesh();

    void getE();
    void getVI_CI();
    // int getVertexIdx(Vertex v);
    // size_t addVertex(Vertex v);
    int addHexCell( size_t v0, size_t v1, size_t v2, size_t v3, 
                    size_t v4, size_t v5, size_t v6, size_t v7);
    void deleteCell(size_t idx);

    void getGeometryInfo();
    void getSurfaceNormal();
};

#endif
