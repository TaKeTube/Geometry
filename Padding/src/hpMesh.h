#ifndef HEX_PADDING_MESH_H
#define HEX_PADDING_MESH_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <eigen3/Eigen/Eigen>

/* Cell related */
enum MeshType {TRIANGLE, QUAD, TETRAHEDRA, HEXAHEDRA, POLYGON};

typedef std::vector<size_t>         Cell;
typedef std::vector<size_t>         Face;
typedef std::pair<size_t, size_t>   Edge;
typedef Eigen::Vector3d             Vert;
typedef std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > Vertexes;

struct CellInfo
{
    // std::vector<size_t> V; // vertexes
    std::vector<size_t> E; // edges
    std::vector<size_t> F; // faces
    bool isBoundary;
};

struct FaceInfo
{
    // std::vector<size_t> V; // vertexes
    std::vector<size_t> E; // edges
    std::vector<size_t> neighborC;    // neighbor cells
    bool isBoundary;
};

struct EdgeInfo
{
    // std::vector<size_t> V; // vertexes
    std::vector<size_t> neighborF;    // neighbor faces
    std::vector<size_t> neighborC;    // neighbor cells
    bool isBoundary;
};

struct VertInfo
{
    // Eigen::Vector3d v; // vertexes
    std::vector<size_t> neighborV;   // neighbor vertexes
    std::vector<size_t> neighborE;   // neighbor edges
    std::vector<size_t> neighborF;   // neighbor faces
    std::vector<size_t> neighborC;   // neighbor cells
    bool isBoundary;

    Eigen::Vector3d normal; // normal vector
    int surfDegree;         // surface degree
    double surfAvgLen;      // surface average length of neighbor edges
};

/* Mesh related */
class Mesh
{
public:
    Vertexes V;
    std::vector<Cell> C;
    std::vector<Edge> E;
    std::vector<Face> F;
    std::unordered_map<size_t, CellInfo> CinfoMap;
    std::unordered_map<size_t, FaceInfo> FinfoMap;
    std::unordered_map<size_t, EdgeInfo> EinfoMap;
    std::unordered_map<size_t, VertInfo> VinfoMap;
    std::vector<size_t> SubV;       // valid vertexes index for submesh
    std::vector<size_t> SurfaceF;
    std::vector<size_t> SurfaceV;
    MeshType cellType;

    Mesh();
    Mesh(const Mesh& mesh);
    // Mesh(const std::vector<Vertex>& v, const std::vector<Cell>& c, const MeshType meshType);
    ~Mesh();

    size_t addVert(Vert &v);
    size_t addCell(Cell &c);
    size_t addCell( size_t v0, size_t v1, size_t v2, size_t v3, 
                    size_t v4, size_t v5, size_t v6, size_t v7);
    // void deleteCell(size_t idx);

    void getGeometryInfo();
    void getSurface();
    void getSurfaceNormal();
    void getSurfaceNormal(Mesh &superMesh);
    void getSurfaceAvgLen();
    void getSurfaceAvgLen(Mesh &superMesh);
};

#endif
