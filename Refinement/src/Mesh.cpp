#include "Mesh.h"
#include "global.h"
#include "Vector.hpp"

enum TemplateCellType {T_INVALID, T_VERT, T_EDGE, T_FACE, T_CELL};

static char faceMask[FACE_NUM] = {0x0F, 0x33, 0x66, 0x99, 0xCC, 0xF0};
static char diagonalMask[FACE_NUM][2] = {{0x05, 0x0A}, {0x21, 0x12}, {0x42, 0x24}, {0x81, 0x18}, {0x84, 0x48}, {0x50, 0xA0}};


/* constructior & destructior for class Vertex */
Vertex::Vertex(float xx) : Vector3f(xx) {}
Vertex::Vertex(float xx, float yy, float zz) : Vector3f(xx, yy, zz) {}
Vertex::Vertex(float xx, float yy, float zz, std::vector<int>& neighbor)
: Vector3f(xx, yy, zz)
, Nbr(neighbor)
{}
Vertex::Vertex(Vector3f v) : Vector3f(v) {}
Vertex::Vertex(){}
Vertex::~Vertex(){}

/* constructior & destructior for class Edge */
Edge::Edge() {}
Edge::Edge(size_t v1, size_t v2) : v1Idx(v1), v2Idx(v2) {}

/* constructior & destructior for class Mesh */
Mesh::Mesh(const std::vector<Vertex>& v, const std::vector<Cell>& c, const CellType cellType)
: V(v)
, C(c)
, cellType(cellType)
{}
Mesh::Mesh(const Mesh& mesh)
: V(mesh.V)
, C(mesh.C)
, cellType(mesh.cellType)
{}
Mesh::Mesh(){}
Mesh::~Mesh(){}

/*
 * getEdgeCenter()
 * DESCRIPTION: get the center of the edge
 * INPUT: edge
 * OUTPUT: center of the edge
 * RETURN: center Vertex of the edge
 */
inline Vertex Mesh::getEdgeCenter(Edge e){
    return (V.at(e.v1Idx)+V.at(e.v2Idx))/2;
}

/*
 * addVertex()
 * DESCRIPTION: add Vertex into the mesh
 * INPUT: vertex to be added
 * OUTPUT: the index of the added vertex
 * RETURN: the index of the added vertex
 */
inline size_t Mesh::addVertex(Vertex v){
    V.push_back(v);
    return V.size()-1;
}

/*
 * addHexCell()
 * DESCRIPTION: add hex cell into the mesh according to indexes of eight vertexes
 * INPUT: indexes of eight vertexes
 * OUTPUT: the index of the added cell
 * RETURN: the index of the added cell
 */
inline int Mesh::addHexCell(size_t v0, size_t v1, size_t v2, size_t v3, 
                            size_t v4, size_t v5, size_t v6, size_t v7){
    Cell c;
    c.push_back(v0);
    c.push_back(v1);
    c.push_back(v2);
    c.push_back(v3);
    c.push_back(v4);
    c.push_back(v5);
    c.push_back(v6);
    c.push_back(v7);
    C.push_back(c);
    return C.size()-1;
}

/*
 * deleteCell()
 * DESCRIPTION: delete the cell in Cell vectors according to corresponding index
 * INPUT: index of the cell to be deleted
 * OUTPUT: none
 * RETURN: none
 */
inline void Mesh::deleteCell(size_t idx){
    auto iter = C.begin()+idx;
    C.erase(iter);
}

/*
 * getE()
 * DESCRIPTION: get edge information of the mesh
 *              stored into a hash map with Edge-EdgeInfo pairs
 * INPUT: hex mesh
 * OUTPUT: a hash map with Edge-EdgeInfo pairs of the hex mesh
 * RETURN: none
 */
void Mesh::getE(){
    for(size_t cellIdx = 0; cellIdx < C.size(); cellIdx++){
        if (cellType == HEXAHEDRA){
            Edge e;
            for (int i = 0; i < HEX_SIZE; i++){
                e.v1Idx = C.at(cellIdx).at(MOD(i, HEX_SIZE));
                e.v2Idx = C.at(cellIdx).at(MOD(i+1, HEX_SIZE));
                E[e].cellIdxVec.push_back(cellIdx);
            }
        }
    }
}

inline int getBitNum(char bitmap){
    return bitNumLookup[bitmap];
}

int Mesh::findTemplateType(size_t cellIdx, std::unordered_map<size_t, CellInfo> &cellInfoMap){
    char Vbitmap = cellInfoMap.at(cellIdx).Vbitmap;
    int Vnum = getBitNum(Vbitmap);

    switch(Vnum)
    {
        case 0:
            return 0x00;
        case 1:
            return Vbitmap;
        case 2:
            for(int i = 0; i < FACE_NUM; i++){
                if(getBitNum(Vbitmap & faceMask[i]) == 2)
                    if(getBitNum(Vbitmap & diagonalMask[i][0]) == 2 || getBitNum(Vbitmap & diagonalMask[i][1]) == 2)
                        return faceMask[i];
                    else
                        return Vbitmap;
            }
            break;
        case 3:
            for(int i = 0; i < FACE_NUM; i++)
                if(getBitNum(Vbitmap & faceMask[i]) == 3)
                    return faceMask[i];
            break;
        case 4:
            for(int i = 0; i < FACE_NUM; i++)
                if(Vbitmap == faceMask[i])
                    return Vbitmap;
            break;
        default:
            break;
    }
    return 0xFF;
}