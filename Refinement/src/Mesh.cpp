#include "Mesh.h"
#include "global.hpp"
#include "Vector.hpp"

enum TemplateCellType {T_INVALID, T_VERT, T_EDGE, T_FACE, T_CELL};

static char faceMask[FACE_NUM] = {0x0F, 0x33, 0x66, 0x99, 0xCC, 0xF0};

static char diagonalMask[FACE_NUM][2] = {
    {0x05, 0x0A}, {0x21, 0x12}, {0x42, 0x24}, 
    {0x81, 0x18}, {0x84, 0x48}, {0x50, 0xA0}
};

static std::unordered_map<char, std::vector<int>> Global2Local {
    /* vertex templates */
    {0x01, {0, 1, 2, 3, 4, 5, 6, 7}}, {0x02, {1, 2, 3, 0, 5, 6, 7, 4}}, 
    {0x04, {2, 3, 0, 1, 6, 7, 4, 5}}, {0x08, {3, 0, 1, 2, 7, 4, 5, 6}},
    {0x10, {4, 7, 6, 5, 0, 3, 2, 1}}, {0x20, {5, 4, 7, 6, 1, 0, 3, 2}},
    {0x40, {6, 5, 4, 7, 2, 1, 0, 3}}, {0x80, {7, 6, 5, 4, 3, 2, 1, 0}},
    /* edge templates */
    {0x03, {0, 1, 2, 3, 4, 5, 6, 7}}, {0x06, {1, 2, 3, 0, 5, 6, 7, 4}}, 
    {0x0C, {2, 3, 0, 1, 6, 7, 4, 5}}, {0x09, {3, 0, 1, 2, 7, 4, 5, 6}},
    {0x90, {4, 7, 6, 5, 0, 3, 2, 1}}, {0x30, {5, 4, 7, 6, 1, 0, 3, 2}},
    {0x60, {6, 5, 4, 7, 2, 1, 0, 3}}, {0xC0, {7, 6, 5, 4, 3, 2, 1, 0}},
    {0x11, {0, 4, 7, 3, 1, 5, 6, 2}}, {0x22, {1, 5, 6, 2, 0, 4, 7, 3}},
    {0x44, {2, 6, 7, 3, 1, 5, 4, 0}}, {0x88, {3, 7, 4, 0, 2, 6, 5, 1}},
    /* face templates */
    {0x0F, {0, 1, 2, 3, 4, 5, 6, 7}}, {0x33, {0, 1, 5, 4, 3, 2, 6, 7}},
    {0x66, {1, 2, 6, 5, 0, 3, 7, 4}}, {0x99, {0, 4, 7, 3, 1, 5, 6, 2}},
    {0xCC, {2, 3, 7, 6, 1, 0, 4, 5}}, {0xF0, {4, 5, 6, 7, 0, 1, 2, 3}}
};

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

void Mesh::getVI_CI()
{
    if(!VI_CI.empty())
        return;
    for(size_t cIdx = 0; cIdx < C.size(); cIdx++){
        for(size_t i = 0; i < C.at(cIdx).size(); i++){
            size_t vIdx = C.at(cIdx).at(i);
            if(!isInVec(cIdx, VI_CI.at(vIdx)))
                VI_CI[vIdx].push_back(cIdx);
        }
    }
}

void Mesh::selectCell(std::vector<size_t> &selectedV, std::vector<size_t> &selectedC){
    for(size_t vIdx = 0; vIdx < selectedV.size(); vIdx++){
        std::vector<size_t> &cVec = VI_CI.at(vIdx);
        for(size_t cIdx = 0; cIdx < cVec.size(); cIdx++){
            /* whether the cell is already selected */
            if(!isInVec(cIdx, selectedC))
                selectedC.push_back(cIdx);
            /* update selected vertex bitmap for the cell */
            size_t i = 0;
            for(i = 0; i < HEX_SIZE && vIdx != C.at(cIdx).at(i); i++);
            cellInfoMap[cIdx].Vbitmap |= (1<<i);
        }
    }
}

char Mesh::getVbitmap(size_t cIdx){
    char Vbitmap = cellInfoMap.at(cIdx).Vbitmap;
    int Vnum = getBitNum(Vbitmap);

    switch(Vnum)
    {
        /* no need for template */
        case 0:
            return 0x00;
        /* vertex template */
        case 1:
            return Vbitmap;
        /* edge or face or cell template */
        case 2:
            for(int i = 0; i < FACE_NUM; i++){
                if(getBitNum(Vbitmap & faceMask[i]) == 2)
                    /* face template */
                    if(getBitNum(Vbitmap & diagonalMask[i][0]) == 2 || getBitNum(Vbitmap & diagonalMask[i][1]) == 2)
                        return faceMask[i];
                    /* edge template */
                    else
                        return Vbitmap;
            }
            /* cell template */
            break;
        /* face or cell template */
        case 3:
            for(int i = 0; i < FACE_NUM; i++)
                /* face template */
                if(getBitNum(Vbitmap & faceMask[i]) == 3)
                    return faceMask[i];
            /* cell template */
            break;
        /* face or cell template */
        case 4:
            for(int i = 0; i < FACE_NUM; i++)
                /* face template */
                if(Vbitmap == faceMask[i])
                    return Vbitmap;
            /* cell template */
            break;
        default:
            break;
    }
    /* cell template */
    return 0xFF;
}

void Mesh::addVertTemplate(Cell c){
    /* get original vertexes */
    size_t v0Idx = c.at(0);
    size_t v1Idx = c.at(1);
    size_t v2Idx = c.at(2);
    size_t v3Idx = c.at(3);
    size_t v4Idx = c.at(4);
    size_t v5Idx = c.at(5);
    size_t v6Idx = c.at(6);
    size_t v7Idx = c.at(7);
    Vertex &v0 = V.at(v0Idx);
    Vertex &v1 = V.at(v1Idx);
    Vertex &v2 = V.at(v2Idx);
    Vertex &v3 = V.at(v3Idx);
    Vertex &v4 = V.at(v4Idx);
    Vertex &v5 = V.at(v5Idx);
    Vertex &v6 = V.at(v6Idx);
    Vertex &v7 = V.at(v7Idx);
    
    /* calculate interpolated points */
    Vector3f xOffset = (v1 - v0) / 3;
    Vector3f yOffset = (v3 - v0) / 3;
    Vector3f zOffset = (v4 - v0) / 3;
    Vertex v8  = v0 + xOffset;
    Vertex v10 = v0 + yOffset;
    Vertex v11 = v0 + zOffset;
    Vertex v9  = v0 + xOffset + yOffset;
    Vertex v12 = v0 + xOffset + zOffset;
    Vertex v14 = v0 + yOffset + zOffset;
    Vertex v13 = v0 + xOffset + yOffset + zOffset;
    
    /* add vertexes */
    size_t v8Idx  = addVertex(v8);
    size_t v9Idx  = addVertex(v9);
    size_t v10Idx = addVertex(v10);
    size_t v11Idx = addVertex(v11);
    size_t v12Idx = addVertex(v12);
    size_t v13Idx = addVertex(v13);
    size_t v14Idx = addVertex(v14);
    
    /* add cells */
    addHexCell(v0Idx,  v8Idx,  v9Idx,  v10Idx, v11Idx, v12Idx, v13Idx, v14Idx);
    addHexCell(v8Idx,  v1Idx,  v2Idx,  v9Idx,  v12Idx, v5Idx,  v6Idx,  v13Idx);
    addHexCell(v9Idx,  v2Idx,  v3Idx,  v12Idx, v13Idx, v6Idx,  v7Idx,  v14Idx);
    addHexCell(v11Idx, v12Idx, v13Idx, v14Idx, v4Idx,  v5Idx,  v6Idx,  v7Idx );
}

void Mesh::addEdgeTemplate(Cell c){
    /* get original vertexes */
    size_t v0Idx = c.at(0);
    size_t v1Idx = c.at(1);
    size_t v2Idx = c.at(2);
    size_t v3Idx = c.at(3);
    size_t v4Idx = c.at(4);
    size_t v5Idx = c.at(5);
    size_t v6Idx = c.at(6);
    size_t v7Idx = c.at(7);
    Vertex &v0 = V.at(v0Idx);
    Vertex &v1 = V.at(v1Idx);
    Vertex &v2 = V.at(v2Idx);
    Vertex &v3 = V.at(v3Idx);
    Vertex &v4 = V.at(v4Idx);
    Vertex &v5 = V.at(v5Idx);
    Vertex &v6 = V.at(v6Idx);
    Vertex &v7 = V.at(v7Idx);

    /* calculate interpolated points */
    Vector3f yOffset1 = (v3 - v0) / 3;
    Vector3f zOffset1 = (v4 - v0) / 3;
    Vector3f yOffset2 = (v2 - v1) / 3;
    Vector3f zOffset2 = (v5 - v1) / 3;
    Vertex v8  = v1 + yOffset2;
    Vertex v9  = v0 + yOffset1;
    Vertex v10 = v0 + zOffset1;
    Vertex v11 = v1 + zOffset2;
    Vertex v12 = v1 + yOffset2 + zOffset2;
    Vertex v13 = v0 + yOffset1 + zOffset1;

    /* add vertexes */
    size_t v8Idx  = addVertex(v8);
    size_t v9Idx  = addVertex(v9);
    size_t v10Idx = addVertex(v10);
    size_t v11Idx = addVertex(v11);
    size_t v12Idx = addVertex(v12);
    size_t v13Idx = addVertex(v13);

    /* add cells */
    addHexCell(v0Idx,  v1Idx,  v8Idx,  v9Idx,  v10Idx, v11Idx, v12Idx, v13Idx);
    addHexCell(v9Idx,  v8Idx,  v2Idx,  v3Idx,  v13Idx, v12Idx, v6Idx,  v7Idx);
    addHexCell(v10Idx, v11Idx, v12Idx, v13Idx, v4Idx,  v5Idx,  v6Idx,  v7Idx);
}

void Mesh::addFaceTemplate(Cell c){
    /* get original vertexes */
    size_t v0Idx = c.at(0);
    size_t v1Idx = c.at(1);
    size_t v2Idx = c.at(2);
    size_t v3Idx = c.at(3);
    size_t v4Idx = c.at(4);
    size_t v5Idx = c.at(5);
    size_t v6Idx = c.at(6);
    size_t v7Idx = c.at(7);
    Vertex &v0 = V.at(v0Idx);
    Vertex &v1 = V.at(v1Idx);
    Vertex &v2 = V.at(v2Idx);
    Vertex &v3 = V.at(v3Idx);
    Vertex &v4 = V.at(v4Idx);
    Vertex &v5 = V.at(v5Idx);
    Vertex &v6 = V.at(v6Idx);
    Vertex &v7 = V.at(v7Idx);

    /* calculate interpolated points */
    Vertex v8  = v0 + (v4 - v0) / 3;
    Vertex v9  = v1 + (v5 - v1) / 3;
    Vertex v10 = v2 + (v6 - v2) / 3;
    Vertex v11 = v3 + (v7 - v3) / 3;

    /* add vertexes */
    size_t v8Idx  = addVertex(v8);
    size_t v9Idx  = addVertex(v9);
    size_t v10Idx = addVertex(v10);
    size_t v11Idx = addVertex(v11);

    /* add cells */
    addHexCell(v0Idx, v1Idx, v2Idx,  v3Idx,  v8Idx, v9Idx, v10Idx, v11Idx);
    addHexCell(v8Idx, v9Idx, v10Idx, v11Idx, v4Idx, v5Idx, v6Idx,  v7Idx );
}

void Mesh::addCellTemplate(Cell c){
    /* get original vertexes */
    size_t v0Idx = c.at(0);
    size_t v1Idx = c.at(1);
    size_t v2Idx = c.at(2);
    size_t v3Idx = c.at(3);
    size_t v4Idx = c.at(4);
    size_t v5Idx = c.at(5);
    size_t v6Idx = c.at(6);
    size_t v7Idx = c.at(7);
    Vertex &v0 = V.at(v0Idx);
    Vertex &v1 = V.at(v1Idx);
    Vertex &v2 = V.at(v2Idx);
    Vertex &v3 = V.at(v3Idx);
    Vertex &v4 = V.at(v4Idx);
    Vertex &v5 = V.at(v5Idx);
    Vertex &v6 = V.at(v6Idx);
    Vertex &v7 = V.at(v7Idx);

    // /* calculate interpolated points */
    // Vertex v8  = v0 + (v4 - v0) / 3;
    // Vertex v9  = v1 + (v5 - v1) / 3;
    // Vertex v10 = v2 + (v6 - v2) / 3;
    // Vertex v11 = v3 + (v7 - v3) / 3;
    // Vertex v12  = v0 + (v4 - v0) / 3;
    // Vertex v13  = v1 + (v5 - v1) / 3;
    // Vertex v14 = v2 + (v6 - v2) / 3;
    // Vertex v15 = v3 + (v7 - v3) / 3;
    // Vertex v16  = v0 + (v4 - v0) / 3;
    // Vertex v17  = v1 + (v5 - v1) / 3;
    // Vertex v18 = v2 + (v6 - v2) / 3;
    // Vertex v19 = v3 + (v7 - v3) / 3;
    // Vertex v20  = v0 + (v4 - v0) / 3;
    // Vertex v21  = v1 + (v5 - v1) / 3;
    // Vertex v22 = v2 + (v6 - v2) / 3;
    // Vertex v23 = v3 + (v7 - v3) / 3;

    // /* add vertexes */
    // size_t v8Idx  = addVertex(v8);
    // size_t v9Idx  = addVertex(v9);
    // size_t v10Idx = addVertex(v10);
    // size_t v11Idx = addVertex(v11);

    // /* add cells */
    // addHexCell(v0Idx, v1Idx, v2Idx,  v3Idx,  v8Idx, v9Idx, v10Idx, v11Idx);
    // addHexCell(v8Idx, v9Idx, v10Idx, v11Idx, v4Idx, v5Idx, v6Idx,  v7Idx );
}

void Mesh::replaceCellWithTemplate(size_t cIdx, char Vbitmap, std::vector<size_t> abandonedCell){
    int Vnum = getBitNum(Vbitmap);
    Cell localc;
    Cell &c = C.at(cIdx);

    /* tranfer global vertex index to local vertex index */
    /* index 0 is the main corner */
    /* 
     *   7 ------- 6    ==vertex==  ==edge==  ==face==
     *  /|        /|    /       / /       / /        /
     * 4 ------- 5 |   --------- ---------  ---------|
     * | 3 ------| 2   |       | |       |  |       ||
     * |/        |/    |===|   | |=======|  |=======||
     * 0 ------- 1     0---|---1 0-------1  0-------1/
     */
    for(size_t idx = 0; idx < HEX_SIZE; idx++)
        localc.push_back(c.at(Global2Local.at(Vbitmap)[idx]));

    switch(Vnum)
    {
        case 0:
            break;
        case 1:
            addVertTemplate(localc);
            abandonedCell.push_back(cIdx);
            break;
        case 2:
            addEdgeTemplate(localc);
            abandonedCell.push_back(cIdx);
            break;
        case 4:
            addFaceTemplate(localc);
            abandonedCell.push_back(cIdx);
            break;
        case 8:
            addCellTemplate(localc);
            abandonedCell.push_back(cIdx);
            break;
        default:
            break;
    }
}

void Mesh::refine(std::vector<size_t> &selectedV){
    std::vector<size_t> selectedC;
    std::vector<size_t> abandonedCell;
    char Vbitmap = 0x00;

    /* select cell according to the selected vertexes */
    selectCell(selectedV, selectedC);
    for(auto c:selectedC){
        Vbitmap = getVbitmap(c);
        replaceCellWithTemplate(c, Vbitmap, abandonedCell);
    }

    /* delete all abandoned cells */
    /* has to be deleted from big to small */
    sort(abandonedCell.rbegin(), abandonedCell.rend());
    for(size_t i = 0; i < abandonedCell.size(); i++)
        deleteCell(abandonedCell.at(i));
}