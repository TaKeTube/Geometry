#include "Mesh.h"
#include "global.h"
#include "Vector.hpp"

Vertex::Vertex(float xx) : Vector3f(xx) {}
Vertex::Vertex(float xx, float yy, float zz) : Vector3f(xx, yy, zz) {}
Vertex::Vertex(float xx, float yy, float zz, std::vector<int>& neighbor)
: Vector3f(xx, yy, zz)
, Nbr(neighbor)
{}
Vertex::Vertex(Vector3f v) : Vector3f(v) {}
Vertex::Vertex(){}
Vertex::~Vertex(){}

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

inline size_t Mesh::addVertex(Vertex v){
    V.push_back(v);
    return V.size();
}

inline void Mesh::addQuadCell(size_t v1, size_t v2, size_t v3, size_t v4){
    Cell c(QUAD_SIZE);
    c.push_back(v1);
    c.push_back(v2);
    c.push_back(v3);
    c.push_back(v4);
    C.push_back(c);
}

inline void Mesh::deleteCell(size_t idx){
    auto iter = C.begin()+idx;
    C.erase(iter);
}

/* 
 *  quad mesh would be split like this:
 * 
 *      v1 -------- v2
 *      | \         |
 *      |  \        |
 *      |   \       |
 *      |    bC---- eC1
 *      |    |      |
 *      |    |      |
 *      v3 -eC2---- v4
 * 
 *  where bC is baryCenter,
 *        eC1 is edgeCenter1,
 *        eC2 is edgeCenter2
 */
int Mesh::removeFlatAngle(){
    size_t cnum = C.size();
    std::vector<size_t> abandonedCell;

    for(size_t i = 0; i < cnum; i++){
        Cell &cell = C.at(i);
        int flatNum = 0;
        /* traverse 4 angles of the quad cell */
        for(size_t i = 0; i < QUAD_SIZE; i++){
            /* get three vertexes of an angle */
            Vertex& v1 = V.at(cell.at(i));
            Vertex& v2 = V.at(cell.at(MOD(i+1, QUAD_SIZE)));
            Vertex& v3 = V.at(cell.at(MOD(i-1, QUAD_SIZE)));

            /* check whether the angle v2-v1-v3 is close to flat angle, if so, remove it by spliting the cell */
            if(cosDist(v2-v1, v3-v1) < -0.75){
                /* add the bad cell index into abandoned cell vector to be deleted */
                abandonedCell.push_back(i);

                /* get the opposite vertex of the flat angle */
                Vertex& v4 = V.at(cell.at(MOD(i+2, QUAD_SIZE)));
                
                /* get the barycenter of the quad cell then add it into the mesh */
                Vertex baryCenter = (v1+v2+v3+v4)/4;
                size_t baryCenterIdx = addVertex(baryCenter);
                
                /* get the center of edge v2-v4 then add it into the mesh */
                Vertex edgeCenter1 = (v2+v4)/2;
                size_t edgeCenter1Idx = addVertex(edgeCenter1);

                /* get the center of edge v3-v4 then add it into the mesh */
                Vertex edgeCenter2 = (v3+v4)/2;
                size_t edgeCenter2Idx = addVertex(edgeCenter2);
                
                /* get the indexes of vertexes of original cell */
                size_t v1Idx = cell.at(i);
                size_t v2Idx = cell.at(MOD(i+1, QUAD_SIZE));
                size_t v3Idx = cell.at(MOD(i-1, QUAD_SIZE));
                size_t v4Idx = cell.at(MOD(i+2, QUAD_SIZE));

                /* add 3 new cells into the mesh */
                addQuadCell(v1Idx, v2Idx, edgeCenter2Idx, baryCenterIdx);
                addQuadCell(v1Idx, baryCenterIdx, edgeCenter2Idx, v3Idx);
                addQuadCell(baryCenterIdx, edgeCenter1Idx, v4Idx, edgeCenter2Idx);

                
            }
        }
    }

    /* delete all abandoned cells */
    for(auto cellIdx : abandonedCell)
        deleteCell(cellIdx);

}