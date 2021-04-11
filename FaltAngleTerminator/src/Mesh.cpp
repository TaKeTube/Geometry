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

Edge::Edge() {}
Edge::Edge(size_t v1, size_t v2) : v1Idx(v1), v2Idx(v2) {}

// EdgeInfo::EdgeInfo() : isSplit(false) {}
// EdgeInfo::EdgeInfo(size_t cellIdx) : isSplit(false){ cellIdxVec.push_back(cellIdx); }

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

inline Vertex Mesh::getEdgeCenter(Edge e){
    return (V.at(e.v1Idx)+V.at(e.v2Idx))/2;
}

Vertex Mesh::getCellCenter(Cell c){
    Vertex center = 0;
    for(size_t i = 0; i < c.size(); i++)
        center += V.at(c.at(i));
    return center;
};

inline size_t Mesh::addVertex(Vertex v){
    V.push_back(v);
    return V.size()-1;
}

inline int Mesh::addQuadCell(size_t v1, size_t v2, size_t v3, size_t v4){
    Cell c;
    c.push_back(v1);
    c.push_back(v2);
    c.push_back(v3);
    c.push_back(v4);
    C.push_back(c);
    return C.size()-1;
}

inline void Mesh::deleteCell(size_t idx){
    auto iter = C.begin()+idx;
    C.erase(iter);
}

void Mesh::getE(){
    for(size_t cellIdx = 0; cellIdx < C.size(); cellIdx++){
        if (cellType == QUAD){
            Edge e;
            for (int i = 0; i < QUAD_SIZE; i++){
                e.v1Idx = C.at(cellIdx).at(MOD(i, QUAD_SIZE));
                e.v2Idx = C.at(cellIdx).at(MOD(i+1, QUAD_SIZE));
                E[e].cellIdxVec.push_back(cellIdx);
            }
        }
    }
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
    std::unordered_map<size_t, CellInfo> cellInfoMap;
    std::queue<size_t> cellQueue;

    for(size_t cellIdx = 0; cellIdx < cnum; cellIdx++){
        Cell &cell = C.at(cellIdx);
        /* traverse 4 angles of the quad cell */
        for(size_t i = 0; i < QUAD_SIZE; i++){
            /* get three vertexes of an angle */
            Vertex v1 = V.at(cell.at(i));
            Vertex v2 = V.at(cell.at(MOD(i+1, QUAD_SIZE)));
            Vertex v3 = V.at(cell.at(MOD(i-1, QUAD_SIZE)));

            /* check whether the angle v2-v1-v3 is close to flat angle, if so, remove it by spliting the cell */
            if(cosDist(v2-v1, v3-v1) < THRESHOLD){

                /* add the bad cell index into abandoned cell vector to be deleted */
                abandonedCell.push_back(cellIdx);
                cellInfoMap[cellIdx].isBadCell = true;

                /* get the opposite vertex of the flat angle */
                Vertex v4 = V.at(cell.at(MOD(i+2, QUAD_SIZE)));

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
                addQuadCell(v1Idx, v2Idx, edgeCenter1Idx, baryCenterIdx);
                addQuadCell(v1Idx, baryCenterIdx, edgeCenter2Idx, v3Idx);
                addQuadCell(baryCenterIdx, edgeCenter1Idx, v4Idx, edgeCenter2Idx);

                /* update edge info */
                Edge e1(v2Idx, v4Idx);
                Edge e2(v3Idx, v4Idx);
                E.at(e1).isSplit = true;
                E.at(e2).isSplit = true;
                E.at(e1).centerIdx = edgeCenter1Idx;
                E.at(e2).centerIdx = edgeCenter2Idx;

                /* push cells to be processed into the cell queue */
                for(auto cIdx : E.at(e1).cellIdxVec)
                    cellQueue.push(cIdx);
                for(auto cIdx : E.at(e2).cellIdxVec)
                    cellQueue.push(cIdx);

                break;
            }
        }
    }

    while(!cellQueue.empty()){
        /* pop the current processing cell */
        size_t curCellIdx = cellQueue.front();
        cellQueue.pop();

        /* if the cell is in cellInfoMap, it could be a bad cell or a split cell */
        if(cellInfoMap.count(curCellIdx)){
            /* get the info of current processing cell */
            CellInfo curCellInfo = cellInfoMap[curCellIdx];
            
            /* if it is a bad cell or it is split twice, end process */
            if(curCellInfo.isBadCell || curCellInfo.splitTime == 2)
                continue;
            
            /* otherwise, the cell must be split once */
            /* get cell information */
            Cell curCell = C.at(curCellIdx);
            std::vector<Edge> curEdge(QUAD_SIZE);
            for(size_t i = 0; i < QUAD_SIZE; i++)
                curEdge.at(i) = Edge(curCell.at(i), curCell.at((i+1)%QUAD_SIZE));

            int splitEdgeNum = 0;
            Edge e;
            Edge oppositeE;
            for(size_t i = 0; i < QUAD_SIZE; i++){
                splitEdgeNum += E.at(curEdge.at(i)).isSplit;
                if(E.at(curEdge.at(i)).isSplit && (!E.at(curEdge.at(MOD(i+2, QUAD_SIZE))).isSplit)){
                    e = curEdge.at(i);
                    oppositeE = curEdge.at(MOD(i+2, QUAD_SIZE));
                }
            }

            /* 
             *  case 1:
             *  ---x---
             *  |  |  |
             *  |  |  |
             *  |  |  |
             *  ---x---
             */
            if(splitEdgeNum == 2){
                continue;
            /* 
             *  case 2:
             *  ---x---
             *  |  |  |
             *  x  |  |
             *  |  |  |
             *  ---x---
             */
            }else if(splitEdgeNum == 3){
                /* clear the original subcell for re-spliting */
                for(auto subC : curCellInfo.subCell)
                    abandonedCell.push_back(subC);

                /* calculate missing point for spliting */
                Vertex oppositeECenter = getEdgeCenter(oppositeE);
                Vertex center = getCellCenter(curCell);

                /* add new vertex */
                int oppositeECenterIdx = addVertex(oppositeECenter);
                int centerIdx = addVertex(center);

                /* update edge and cell info */
                E[oppositeE].isSplit = true;
                E[oppositeE].centerIdx = oppositeECenterIdx;
                cellInfoMap[curCellIdx].splitTime++;
                

                /* add new cells */
                addQuadCell(curCell.at(0), E[curEdge.at(0)].centerIdx, centerIdx, E[curEdge.at(3)].centerIdx);
                addQuadCell(E[curEdge.at(0)].centerIdx, curCell.at(1), E[curEdge.at(1)].centerIdx, centerIdx);
                addQuadCell(centerIdx, E[curEdge.at(1)].centerIdx, curCell.at(2), E[curEdge.at(2)].centerIdx);
                addQuadCell(E[curEdge.at(3)].centerIdx, centerIdx, E[curEdge.at(2)].centerIdx, curCell.at(3));

                /* push cell to be processed into the queue */
                for(auto cIdx : E.at(oppositeE).cellIdxVec)
                    cellQueue.push(cIdx);
            /* 
             *  case 3:
             *  ---x---
             *  |  |  |
             *  x  |  x
             *  |  |  |
             *  ---x---
             */
            }else if(splitEdgeNum == 4){
                /* clear the original subcell for re-spliting */
                for(auto subC : curCellInfo.subCell)
                    abandonedCell.push_back(subC);
                
                /* calculate missing point for spliting */
                Vertex center = getCellCenter(curCell);

                /* add new vertex */
                int centerIdx = addVertex(center);

                /* update cell info */
                cellInfoMap[curCellIdx].splitTime++;

                /* add new cells */
                addQuadCell(curCell.at(0), E[curEdge.at(0)].centerIdx, centerIdx, E[curEdge.at(3)].centerIdx);
                addQuadCell(E[curEdge.at(0)].centerIdx, curCell.at(1), E[curEdge.at(1)].centerIdx, centerIdx);
                addQuadCell(centerIdx, E[curEdge.at(1)].centerIdx, curCell.at(2), E[curEdge.at(2)].centerIdx);
                addQuadCell(E[curEdge.at(3)].centerIdx, centerIdx, E[curEdge.at(2)].centerIdx, curCell.at(3));
            }else{}
        /* if the cell is not in the cell info map, then it must not be a bad cell or be split */
        }else{
            /* get cell information */
            CellInfo curCellInfo = cellInfoMap[curCellIdx];
            Cell curCell = C.at(curCellIdx);
            std::vector<Edge> curEdge(QUAD_SIZE);
            for(size_t i = 0; i < QUAD_SIZE; i++)
                curEdge.at(i) = Edge(curCell.at(i), curCell.at((i+1)%QUAD_SIZE));

            /* the cell should be split, delete the original cell */
            abandonedCell.push_back(curCellIdx);

            int splitEdgeNum = 0;
            Edge e;
            Edge oppositeE;
            for(size_t i = 0; i < QUAD_SIZE; i++){
                splitEdgeNum += E.at(curEdge.at(i)).isSplit;
                if(E.at(curEdge.at(i)).isSplit){
                    e = curEdge.at(i);
                    oppositeE = curEdge.at(MOD(i+2, QUAD_SIZE));
                }
            }

            if(splitEdgeNum == 1 || (splitEdgeNum == 2 && !E[oppositeE].isSplit)){
                /* calculate missing point for spliting */
                Vertex oppositeECenter = getEdgeCenter(oppositeE);

                /* add new vertex */
                int oppositeECenterIdx = addVertex(oppositeECenter);

                /* update edge and cell info */
                E[oppositeE].isSplit = true;
                E[oppositeE].centerIdx = oppositeECenterIdx;
                cellInfoMap[curCellIdx].splitTime++;

                /* add new cells */
                cellInfoMap[curCellIdx].subCell.push_back(
                    addQuadCell(e.v1Idx, E[e].centerIdx, E[oppositeE].centerIdx, oppositeE.v2Idx)
                );
                cellInfoMap[curCellIdx].subCell.push_back(
                    addQuadCell(E[e].centerIdx, e.v2Idx, oppositeE.v1Idx, E[oppositeE].centerIdx)
                );

                /* push cell to be processed into the queue */
                for(auto cIdx : E.at(oppositeE).cellIdxVec)
                    cellQueue.push(cIdx);

            }else if(splitEdgeNum == 2){
                /* update cell info */
                cellInfoMap[curCellIdx].splitTime++;

                /* add new cells */
                cellInfoMap[curCellIdx].subCell.push_back(
                    addQuadCell(e.v1Idx, E[e].centerIdx, E[oppositeE].centerIdx, oppositeE.v2Idx)
                );
                cellInfoMap[curCellIdx].subCell.push_back(
                    addQuadCell(E[e].centerIdx, e.v2Idx, oppositeE.v1Idx, E[oppositeE].centerIdx)
                );

            }else if(splitEdgeNum == 3){
                for(size_t i = 0; i < QUAD_SIZE; i++){
                    if(E.at(curEdge.at(i)).isSplit && (!E.at(curEdge.at(MOD(i+2, QUAD_SIZE))).isSplit)){
                        e = curEdge.at(i);
                        oppositeE = curEdge.at(MOD(i+2, QUAD_SIZE));
                    }
                }

                /* calculate missing point for spliting */
                Vertex oppositeECenter = getEdgeCenter(oppositeE);
                Vertex center = getCellCenter(curCell);

                /* add new vertex */
                int oppositeECenterIdx = addVertex(oppositeECenter);
                int centerIdx = addVertex(center);

                /* update edge and cell info */
                E[oppositeE].isSplit = true;
                E[oppositeE].centerIdx = oppositeECenterIdx;
                cellInfoMap[curCellIdx].splitTime = 2;

                /* add new cells */
                addQuadCell(curCell.at(0), E[curEdge.at(0)].centerIdx, centerIdx, E[curEdge.at(3)].centerIdx);
                addQuadCell(E[curEdge.at(0)].centerIdx, curCell.at(1), E[curEdge.at(1)].centerIdx, centerIdx);
                addQuadCell(centerIdx, E[curEdge.at(1)].centerIdx, curCell.at(2), E[curEdge.at(2)].centerIdx);
                addQuadCell(E[curEdge.at(3)].centerIdx, centerIdx, E[curEdge.at(2)].centerIdx, curCell.at(3));

                /* push cell to be processed into the queue */
                for(auto cIdx : E.at(oppositeE).cellIdxVec)
                    cellQueue.push(cIdx);

            }else if(splitEdgeNum == 4){
                /* calculate missing point for spliting */
                Vertex center = getCellCenter(curCell);

                /* add new vertex */
                int centerIdx = addVertex(center);

                /* update cell info */
                cellInfoMap[curCellIdx].splitTime = 2;

                /* add new cells */
                addQuadCell(curCell.at(0), E[curEdge.at(0)].centerIdx, centerIdx, E[curEdge.at(3)].centerIdx);
                addQuadCell(E[curEdge.at(0)].centerIdx, curCell.at(1), E[curEdge.at(1)].centerIdx, centerIdx);
                addQuadCell(centerIdx, E[curEdge.at(1)].centerIdx, curCell.at(2), E[curEdge.at(2)].centerIdx);
                addQuadCell(E[curEdge.at(3)].centerIdx, centerIdx, E[curEdge.at(2)].centerIdx, curCell.at(3));
            }else{}
        }
    }

    /* delete all abandoned cells */
    // for(int i = abandonedCell.size()-1; i >=0; i--)
    //     deleteCell(abandonedCell.at(i));
    sort(abandonedCell.rbegin(), abandonedCell.rend());
    for(size_t i = 0; i < abandonedCell.size(); i++)
        deleteCell(abandonedCell.at(i));

    return 0;
}