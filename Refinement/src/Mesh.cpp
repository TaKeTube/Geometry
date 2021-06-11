#include "Mesh.h"
#include "global.hpp"
#include "Vector.hpp"

enum TemplateCellType {T_INVALID, T_VERT, T_EDGE, T_FACE, T_CELL};

static unsigned char faceMask[FACE_NUM] = {0x0F, 0x33, 0x66, 0x99, 0xCC, 0xF0};

static unsigned char diagonalMask[FACE_NUM][2] = {
    {0x05, 0x0A}, {0x21, 0x12}, {0x42, 0x24},
    {0x81, 0x18}, {0x84, 0x48}, {0x50, 0xA0}
};

static std::unordered_map<unsigned char, std::vector<int>> Global2Local {
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
            if(!isInMap(vIdx, VI_CI) || !isInVec(cIdx, VI_CI.at(vIdx)))
                VI_CI[vIdx].push_back(cIdx);
        }
    }
}

void Mesh::selectCell(std::vector<size_t> &selectedV, std::vector<size_t> &selectedC){
    for(auto vIdx : selectedV){
        std::vector<size_t> &cVec = VI_CI.at(vIdx);
        for(auto cIdx : cVec){
            /* whether the cell is already selected */
            if(!isInVec(cIdx, selectedC))
                selectedC.push_back(cIdx);
            /* update selected vertex bitmap for the cell */
            size_t i = 0;
            for(i = 0; i < HEX_SIZE && (int)vIdx != C.at(cIdx).at(i); i++);
            cellInfoMap[cIdx].Vbitmap |= (1<<i);
        }
    }
}

void Mesh::removeConcavity(std::vector<size_t> &selectedC){
    unsigned char Vbitmap, newVbitmap;
    bool hasConcavity = true;

    /* update cell selected configuration until there is no concavity (no standard configration) */
    while(hasConcavity){
        std::vector<size_t> newC;
        hasConcavity = false;

        /* check whether there is concavity in current loop */
        for(auto cIdx : selectedC){
            /* get original Vertex bitmap */
            Vbitmap = cellInfoMap.at(cIdx).Vbitmap;
            /* get standard Vertex bitmap */
            newVbitmap = getVbitmap(cIdx);

            /* Two bitmap dismatching means there is concavity       *
             * (more generally, nonstandard selection configuration) */
            if(newVbitmap != Vbitmap){
                cellInfoMap.at(cIdx).Vbitmap = newVbitmap;
                hasConcavity = true;
                /*  select new vertexes */
                std::vector<size_t> newV;
                for(int j = 0; j < HEX_SIZE; j++){
                    if(((char)1<<j) & newVbitmap)
                        newV.push_back(C.at(cIdx).at(j));
                }
                /* select new cells according to new vertexes */
                // selectCell(newV, newC);
                selectCell(newV, selectedC);
            }
        }

        /* update selected cells */
        for(auto cIdx : newC)
            // if(!isInVec(cIdx, selectedC))
            selectedC.push_back(cIdx);
    }
}

unsigned char Mesh::getVbitmap(size_t cIdx){
    unsigned char Vbitmap = cellInfoMap.at(cIdx).Vbitmap;
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
                if(getBitNum(Vbitmap & faceMask[i]) == 2){
                    /* face template */
                    if(getBitNum(Vbitmap & diagonalMask[i][0]) == 2 || getBitNum(Vbitmap & diagonalMask[i][1]) == 2)
                        return faceMask[i];
                    /* edge template */
                    else
                        return Vbitmap;
                }
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

void Mesh::addModifiedEdgeTemplate(Cell c){
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
    // Vertex &v6 = V.at(v6Idx);
    // Vertex &v7 = V.at(v7Idx);

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

void Mesh::addModifiedFaceTemplate(Cell c){
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
    // Vertex &v2 = V.at(v2Idx);
    Vertex &v3 = V.at(v3Idx);
    Vertex &v4 = V.at(v4Idx);
    // Vertex &v5 = V.at(v5Idx);
    // Vertex &v6 = V.at(v6Idx);
    // Vertex &v7 = V.at(v7Idx);
    
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
    addHexCell(v10Idx, v9Idx,  v2Idx,  v3Idx,  v14Idx, v13Idx, v6Idx,  v7Idx );
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

    /* calculate interpolated points after modified parallel refinement */
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

    /* calculate interpolated points after single sheet refinement */
    Vertex n0  = v0  + (v1  - v0 ) / 3;
    Vertex n1  = v1  - (v1  - v0 ) / 3;
    Vertex n2  = v8  + (v9  - v8 ) / 3;
    Vertex n3  = v9  - (v9  - v8 ) / 3;
    Vertex n4  = v10 + (v11 - v10) / 3;
    Vertex n5  = v11 - (v11 - v10) / 3;
    Vertex n6  = v12 + (v13 - v12) / 3;
    Vertex n7  = v13 - (v13 - v12) / 3;
    Vertex n8  = ((v4 + (v5 - v4) / 3) + n4) * 0.5;
    Vertex n9  = ((v5 - (v5 - v4) / 3) + n5) * 0.5;
    Vertex n10 = ((v3 - (v3 - v2) / 3) + n3) * 0.5;
    Vertex n11 = ((v2 + (v3 - v2) / 3) + n2) * 0.5;
    Vertex n12 = ((v7 - (v7 - v6) / 3) + n7) * 0.5;
    Vertex n13 = ((v6 + (v7 - v6) / 3) + n6) * 0.5;

    /* add vertexes */
    size_t v8Idx  = addVertex(v8);
    size_t v9Idx  = addVertex(v9);
    size_t v10Idx = addVertex(v10);
    size_t v11Idx = addVertex(v11);
    size_t v12Idx = addVertex(v12);
    size_t v13Idx = addVertex(v13);
    size_t n0Idx  = addVertex(n0);
    size_t n1Idx  = addVertex(n1);
    size_t n2Idx  = addVertex(n2);
    size_t n3Idx  = addVertex(n3);
    size_t n4Idx  = addVertex(n4);
    size_t n5Idx  = addVertex(n5);
    size_t n6Idx  = addVertex(n6);
    size_t n7Idx  = addVertex(n7);
    size_t n8Idx  = addVertex(n8);
    size_t n9Idx  = addVertex(n9);
    size_t n10Idx = addVertex(n10);
    size_t n11Idx = addVertex(n11);
    size_t n12Idx = addVertex(n12);
    size_t n13Idx = addVertex(n13);

    /* add cells */
    addHexCell(v0Idx,  n0Idx,  n3Idx,  v9Idx,  v10Idx, n4Idx,  n7Idx,  v13Idx);
    addHexCell(n0Idx,  n1Idx,  n2Idx,  n3Idx,  n4Idx,  n5Idx,  n6Idx,  n7Idx );
    addHexCell(n1Idx,  v1Idx,  v8Idx,  n2Idx,  n5Idx,  v11Idx, v12Idx, n6Idx );
    addHexCell(v10Idx, n4Idx,  n7Idx,  v13Idx, v4Idx,  n8Idx,  n12Idx, v7Idx );
    addHexCell(n4Idx,  n5Idx,  n6Idx,  n7Idx,  n8Idx,  n9Idx,  n13Idx, n12Idx);
    addHexCell(n5Idx,  v11Idx, v12Idx, n6Idx,  n9Idx,  v5Idx,  v6Idx,  n13Idx);
    addHexCell(n8Idx,  n9Idx,  n13Idx, n12Idx, v4Idx,  v5Idx,  v6Idx,  v7Idx );
    addHexCell(v9Idx,  n3Idx,  n10Idx, v3Idx,  v13Idx, n7Idx,  n12Idx, v7Idx );
    addHexCell(n3Idx,  n2Idx,  n11Idx, n10Idx, n7Idx,  n6Idx,  n13Idx, n12Idx);
    addHexCell(n2Idx,  v8Idx,  v2Idx,  n11Idx, n6Idx,  v12Idx, v6Idx,  n13Idx);
    addHexCell(n10Idx, n11Idx, v2Idx,  v3Idx,  n12Idx, n13Idx, v6Idx,  v7Idx );
}

void Mesh::addFaceTemplate(Cell c){
    /* get original vertexes */
    size_t v0Idx  = c.at(0);
    size_t v3Idx  = c.at(1);
    size_t v15Idx = c.at(2);
    size_t v12Idx = c.at(3);
    size_t v48Idx = c.at(4);
    size_t v51Idx = c.at(5);
    size_t v63Idx = c.at(6);
    size_t v60Idx = c.at(7);
    Vertex &v0  = V.at(v0Idx);
    Vertex &v3  = V.at(v3Idx);
    Vertex &v12 = V.at(v12Idx);
    Vertex &v15 = V.at(v15Idx);
    Vertex &v48 = V.at(v48Idx);
    Vertex &v51 = V.at(v51Idx);
    Vertex &v60 = V.at(v60Idx);
    Vertex &v63 = V.at(v63Idx);

    /* calculate 27 subdivisions points */
    /* first layer */
    // Vertex v0  = ;
    // Vertex v3  = ;
    // Vertex v12 = ;
    // Vertex v15 = ;
    Vertex v1  = v0  + (v3  - v0) / 3;
    Vertex v2  = v3  - (v3  - v0) / 3;
    Vertex v4  = v0  + (v12 - v0) / 3;
    Vertex v7  = v3  + (v15 - v3) / 3;
    Vertex v5  = v4  + (v7  - v4) / 3;
    Vertex v6  = v7  - (v7  - v4) / 3;
    Vertex v8  = v12 - (v12 - v0) / 3;
    Vertex v11 = v15 - (v15 - v3) / 3;
    Vertex v9  = v8  + (v11 - v8) / 3;
    Vertex v10 = v11 - (v11 - v8) / 3;
    Vertex v13 = v12 + (v15 - v12) / 3;
    Vertex v14 = v15 - (v15 - v12) / 3;
    /* second layer */
    Vertex v16 = v0  + (v48 - v0 ) / 3; 
    Vertex v19 = v3  + (v51 - v3 ) / 3;
    Vertex v28 = v12 + (v60 - v12) / 3;
    Vertex v31 = v15 + (v63 - v15) / 3;
    Vertex v17 = v16 + (v19 - v16) / 3;
    Vertex v18 = v19 - (v19 - v16) / 3;
    Vertex v20 = v16 + (v28 - v16) / 3;
    Vertex v23 = v19 + (v31 - v19) / 3;
    Vertex v21 = v20 + (v23 - v20) / 3;
    Vertex v22 = v23 - (v23 - v20) / 3;
    Vertex v24 = v28 - (v28 - v16) / 3;
    Vertex v27 = v31 - (v31 - v19) / 3;
    Vertex v25 = v24 + (v27 - v24) / 3;
    Vertex v26 = v27 - (v27 - v24) / 3;
    Vertex v29 = v28 + (v31 - v28) / 3;
    Vertex v30 = v31 - (v31 - v28) / 3;
    /* third layer */
    Vertex v32 = v48 - (v48 - v0 ) / 3;
    Vertex v35 = v51 - (v51 - v3 ) / 3;
    Vertex v44 = v60 - (v60 - v12) / 3;
    Vertex v47 = v63 - (v63 - v15) / 3;
    Vertex v33 = v32 + (v35 - v32) / 3;
    Vertex v34 = v35 - (v35 - v32) / 3;
    Vertex v36 = v32 + (v44 - v32) / 3;
    Vertex v39 = v35 + (v47 - v35) / 3;
    // Vertex v37 = v36 + (v39 - v36) / 3;
    // Vertex v38 = v39 - (v39 - v36) / 3;
    Vertex v40 = v44 - (v44 - v32) / 3;
    Vertex v43 = v47 - (v47 - v35) / 3;
    // Vertex v41 = v40 + (v43 - v40) / 3;
    // Vertex v42 = v43 - (v43 - v40) / 3;
    Vertex v45 = v44 + (v47 - v44) / 3;
    Vertex v46 = v47 - (v47 - v44) / 3;
    /* forth layer */
    // Vertex v48  = ;
    // Vertex v51  = ;
    // Vertex v60 = ;
    // Vertex v63 = ;

    Vertex n0 = intersectRayPlane(v48, v36, v33, v5,  v21 - v5 );
    Vertex n1 = intersectRayPlane(v51, v34, v39, v6,  v22 - v6 );
    Vertex n2 = intersectRayPlane(v63, v43, v46, v10, v26 - v10);
    Vertex n3 = intersectRayPlane(v60, v45, v40, v9,  v25 - v9 );

    /* add vertexes */
    size_t v1Idx  = addVertex(v1);
    size_t v2Idx  = addVertex(v2);
    size_t v4Idx  = addVertex(v4);
    size_t v7Idx  = addVertex(v7);
    size_t v5Idx  = addVertex(v5);
    size_t v6Idx  = addVertex(v6);
    size_t v8Idx  = addVertex(v8);
    size_t v11Idx = addVertex(v11);
    size_t v9Idx  = addVertex(v9);
    size_t v10Idx = addVertex(v10);
    size_t v13Idx = addVertex(v13);
    size_t v14Idx = addVertex(v14);

    size_t v16Idx = addVertex(v16);
    size_t v19Idx = addVertex(v19);
    size_t v28Idx = addVertex(v28);
    size_t v31Idx = addVertex(v31);
    size_t v17Idx = addVertex(v17);
    size_t v18Idx = addVertex(v18);
    size_t v20Idx = addVertex(v20);
    size_t v23Idx = addVertex(v23);
    size_t v21Idx = addVertex(v21);
    size_t v22Idx = addVertex(v22);
    size_t v24Idx = addVertex(v24);
    size_t v27Idx = addVertex(v27);
    size_t v25Idx = addVertex(v25);
    size_t v26Idx = addVertex(v26);
    size_t v29Idx = addVertex(v29);
    size_t v30Idx = addVertex(v30);

    size_t v32Idx = addVertex(v32);
    size_t v35Idx = addVertex(v35);
    size_t v44Idx = addVertex(v44);
    size_t v47Idx = addVertex(v47);
    size_t v33Idx = addVertex(v33);
    size_t v34Idx = addVertex(v34);
    size_t v36Idx = addVertex(v36);
    size_t v39Idx = addVertex(v39);
    // size_t v37Idx = addVertex(v37);
    // size_t v38Idx = addVertex(v38);
    size_t v40Idx = addVertex(v40);
    size_t v43Idx = addVertex(v43);
    // size_t v41Idx = addVertex(v41);
    // size_t v42Idx = addVertex(v42);
    size_t v45Idx = addVertex(v45);
    size_t v46Idx = addVertex(v46);

    size_t n0Idx  = addVertex(n0);
    size_t n1Idx  = addVertex(n1);
    size_t n2Idx  = addVertex(n2);
    size_t n3Idx  = addVertex(n3);

    /* add cells */ 
    /* first layer */
    addHexCell(v0Idx,  v1Idx,  v5Idx,  v4Idx,  v16Idx, v17Idx, v21Idx, v20Idx);
    addHexCell(v1Idx,  v2Idx,  v6Idx,  v5Idx,  v17Idx, v18Idx, v22Idx, v21Idx);
    addHexCell(v2Idx,  v3Idx,  v7Idx,  v6Idx,  v18Idx, v19Idx, v23Idx, v22Idx);
    addHexCell(v4Idx,  v5Idx,  v9Idx,  v8Idx,  v20Idx, v21Idx, v25Idx, v24Idx);
    addHexCell(v5Idx,  v6Idx,  v10Idx, v9Idx,  v21Idx, v22Idx, v26Idx, v25Idx);
    addHexCell(v6Idx,  v7Idx,  v11Idx, v10Idx, v22Idx, v23Idx, v27Idx, v26Idx);
    addHexCell(v8Idx,  v9Idx,  v13Idx, v12Idx, v24Idx, v25Idx, v29Idx, v28Idx);
    addHexCell(v9Idx,  v10Idx, v14Idx, v13Idx, v25Idx, v26Idx, v30Idx, v29Idx);
    addHexCell(v10Idx, v11Idx, v15Idx, v14Idx, v26Idx, v27Idx, v31Idx, v30Idx);

    /* four tall pillars at edges */
    addHexCell(v16Idx, v17Idx, v21Idx, v20Idx, v48Idx, v33Idx, n0Idx,  v36Idx);
    addHexCell(v18Idx, v19Idx, v23Idx, v22Idx, v34Idx, v51Idx, v39Idx, n1Idx );
    addHexCell(v24Idx, v25Idx, v29Idx, v28Idx, v40Idx, n3Idx,  v45Idx, v60Idx);
    addHexCell(v26Idx, v27Idx, v31Idx, v30Idx, n2Idx,  v43Idx, v63Idx, v46Idx);

    /* four short pillars in middle */
    addHexCell(v17Idx, v18Idx, v22Idx, v21Idx, v33Idx, v34Idx, n1Idx,  n0Idx );
    addHexCell(v20Idx, v21Idx, v25Idx, v24Idx, v36Idx, n0Idx,  n3Idx,  v40Idx);
    addHexCell(v22Idx, v23Idx, v27Idx, v26Idx, n1Idx,  v39Idx, v43Idx, n2Idx );
    addHexCell(v25Idx, v26Idx, v30Idx, v29Idx, n3Idx,  n2Idx,  v46Idx, v45Idx);

    /* central small hex */
    addHexCell(v21Idx, v22Idx, v26Idx, v25Idx, n0Idx,  n1Idx,  n2Idx,  n3Idx );

    /* upper structures */
    addHexCell(v33Idx, v34Idx, n1Idx,  n0Idx,  v48Idx, v51Idx, v39Idx, v36Idx);
    addHexCell(n3Idx,  n2Idx,  v46Idx, v45Idx, v40Idx, v43Idx, v63Idx, v60Idx);
    addHexCell(n0Idx,  n1Idx,  n2Idx,  n3Idx , v20Idx, v23Idx, v27Idx, v24Idx);
    addHexCell(v20Idx, v23Idx, v27Idx, v24Idx, v48Idx, v51Idx, v63Idx, v60Idx);
}

void Mesh::addCellTemplate(Cell c){
    /* get original vertexes */
    size_t v0Idx  = c.at(0);
    size_t v3Idx  = c.at(1);
    size_t v15Idx = c.at(2);
    size_t v12Idx = c.at(3);
    size_t v48Idx = c.at(4);
    size_t v51Idx = c.at(5);
    size_t v63Idx = c.at(6);
    size_t v60Idx = c.at(7);
    Vertex &v0  = V.at(v0Idx);
    Vertex &v3  = V.at(v3Idx);
    Vertex &v12 = V.at(v12Idx);
    Vertex &v15 = V.at(v15Idx);
    Vertex &v48 = V.at(v48Idx);
    Vertex &v51 = V.at(v51Idx);
    Vertex &v60 = V.at(v60Idx);
    Vertex &v63 = V.at(v63Idx);

    /* calculate interpolated points */
    /* first layer */
    // Vertex v0  = ;
    // Vertex v3  = ;
    // Vertex v12 = ;
    // Vertex v15 = ;
    Vertex v1  = v0  + (v3  - v0) / 3;
    Vertex v2  = v3  - (v3  - v0) / 3;
    Vertex v4  = v0  + (v12 - v0) / 3;
    Vertex v7  = v3  + (v15 - v3) / 3;
    Vertex v5  = v4  + (v7  - v4) / 3;
    Vertex v6  = v7  - (v7  - v4) / 3;
    Vertex v8  = v12 - (v12 - v0) / 3;
    Vertex v11 = v15 - (v15 - v3) / 3;
    Vertex v9  = v8  + (v11 - v8) / 3;
    Vertex v10 = v11 - (v11 - v8) / 3;
    Vertex v13 = v12 + (v15 - v12) / 3;
    Vertex v14 = v15 - (v15 - v12) / 3;
    /* second layer */
    Vertex v16 = v0  + (v48 - v0 ) / 3; 
    Vertex v19 = v3  + (v51 - v3 ) / 3;
    Vertex v28 = v12 + (v60 - v12) / 3;
    Vertex v31 = v15 + (v63 - v15) / 3;
    Vertex v17 = v16 + (v19 - v16) / 3;
    Vertex v18 = v19 - (v19 - v16) / 3;
    Vertex v20 = v16 + (v28 - v16) / 3;
    Vertex v23 = v19 + (v31 - v19) / 3;
    Vertex v21 = v20 + (v23 - v20) / 3;
    Vertex v22 = v23 - (v23 - v20) / 3;
    Vertex v24 = v28 - (v28 - v16) / 3;
    Vertex v27 = v31 - (v31 - v19) / 3;
    Vertex v25 = v24 + (v27 - v24) / 3;
    Vertex v26 = v27 - (v27 - v24) / 3;
    Vertex v29 = v28 + (v31 - v28) / 3;
    Vertex v30 = v31 - (v31 - v28) / 3;
    /* third layer */
    Vertex v32 = v48 - (v48 - v0 ) / 3;
    Vertex v35 = v51 - (v51 - v3 ) / 3;
    Vertex v44 = v60 - (v60 - v12) / 3;
    Vertex v47 = v63 - (v63 - v15) / 3;
    Vertex v33 = v32 + (v35 - v32) / 3;
    Vertex v34 = v35 - (v35 - v32) / 3;
    Vertex v36 = v32 + (v44 - v32) / 3;
    Vertex v39 = v35 + (v47 - v35) / 3;
    Vertex v37 = v36 + (v39 - v36) / 3;
    Vertex v38 = v39 - (v39 - v36) / 3;
    Vertex v40 = v44 - (v44 - v32) / 3;
    Vertex v43 = v47 - (v47 - v35) / 3;
    Vertex v41 = v40 + (v43 - v40) / 3;
    Vertex v42 = v43 - (v43 - v40) / 3;
    Vertex v45 = v44 + (v47 - v44) / 3;
    Vertex v46 = v47 - (v47 - v44) / 3;
    /* forth layer */
    // Vertex v48  = ;
    // Vertex v51  = ;
    // Vertex v60 = ;
    // Vertex v63 = ;
    Vertex v49 = v48 + (v51 - v48) / 3;
    Vertex v50 = v51 - (v51 - v48) / 3;
    Vertex v52 = v48 + (v60 - v48) / 3;
    Vertex v55 = v51 + (v63 - v51) / 3;
    Vertex v53 = v52 + (v55 - v52) / 3;
    Vertex v54 = v55 - (v55 - v52) / 3;
    Vertex v56 = v60 - (v60 - v48) / 3;
    Vertex v59 = v63 - (v63 - v51) / 3;
    Vertex v57 = v56 + (v59 - v56) / 3;
    Vertex v58 = v59 - (v59 - v56) / 3;
    Vertex v61 = v60 + (v63 - v60) / 3;
    Vertex v62 = v63 - (v63 - v60) / 3;

    /* add vertexes */
    size_t v1Idx  = addVertex(v1);
    size_t v2Idx  = addVertex(v2);
    size_t v4Idx  = addVertex(v4);
    size_t v7Idx  = addVertex(v7);
    size_t v5Idx  = addVertex(v5);
    size_t v6Idx  = addVertex(v6);
    size_t v8Idx  = addVertex(v8);
    size_t v11Idx = addVertex(v11);
    size_t v9Idx  = addVertex(v9);
    size_t v10Idx = addVertex(v10);
    size_t v13Idx = addVertex(v13);
    size_t v14Idx = addVertex(v14);

    size_t v16Idx = addVertex(v16);
    size_t v19Idx = addVertex(v19);
    size_t v28Idx = addVertex(v28);
    size_t v31Idx = addVertex(v31);
    size_t v17Idx = addVertex(v17);
    size_t v18Idx = addVertex(v18);
    size_t v20Idx = addVertex(v20);
    size_t v23Idx = addVertex(v23);
    size_t v21Idx = addVertex(v21);
    size_t v22Idx = addVertex(v22);
    size_t v24Idx = addVertex(v24);
    size_t v27Idx = addVertex(v27);
    size_t v25Idx = addVertex(v25);
    size_t v26Idx = addVertex(v26);
    size_t v29Idx = addVertex(v29);
    size_t v30Idx = addVertex(v30);

    size_t v32Idx = addVertex(v32);
    size_t v35Idx = addVertex(v35);
    size_t v44Idx = addVertex(v44);
    size_t v47Idx = addVertex(v47);
    size_t v33Idx = addVertex(v33);
    size_t v34Idx = addVertex(v34);
    size_t v36Idx = addVertex(v36);
    size_t v39Idx = addVertex(v39);
    size_t v37Idx = addVertex(v37);
    size_t v38Idx = addVertex(v38);
    size_t v40Idx = addVertex(v40);
    size_t v43Idx = addVertex(v43);
    size_t v41Idx = addVertex(v41);
    size_t v42Idx = addVertex(v42);
    size_t v45Idx = addVertex(v45);
    size_t v46Idx = addVertex(v46);

    size_t v49Idx = addVertex(v49);
    size_t v50Idx = addVertex(v50);
    size_t v52Idx = addVertex(v52);
    size_t v55Idx = addVertex(v55);
    size_t v53Idx = addVertex(v53);
    size_t v54Idx = addVertex(v54);
    size_t v56Idx = addVertex(v56);
    size_t v59Idx = addVertex(v59);
    size_t v57Idx = addVertex(v57);
    size_t v58Idx = addVertex(v58);
    size_t v61Idx = addVertex(v61);
    size_t v62Idx = addVertex(v62);

    /* add cells */ 
    addHexCell(v0Idx,  v1Idx,  v5Idx,  v4Idx,  v16Idx, v17Idx, v21Idx, v20Idx);
    addHexCell(v1Idx,  v2Idx,  v6Idx,  v5Idx,  v17Idx, v18Idx, v22Idx, v21Idx);
    addHexCell(v2Idx,  v3Idx,  v7Idx,  v6Idx,  v18Idx, v19Idx, v23Idx, v22Idx);
    addHexCell(v4Idx,  v5Idx,  v9Idx,  v8Idx,  v20Idx, v21Idx, v25Idx, v24Idx);
    addHexCell(v5Idx,  v6Idx,  v10Idx, v9Idx,  v21Idx, v22Idx, v26Idx, v25Idx);
    addHexCell(v6Idx,  v7Idx,  v11Idx, v10Idx, v22Idx, v23Idx, v27Idx, v26Idx);
    addHexCell(v8Idx,  v9Idx,  v13Idx, v12Idx, v24Idx, v25Idx, v29Idx, v28Idx);
    addHexCell(v9Idx,  v10Idx, v14Idx, v13Idx, v25Idx, v26Idx, v30Idx, v29Idx);
    addHexCell(v10Idx, v11Idx, v15Idx, v14Idx, v26Idx, v27Idx, v31Idx, v30Idx);
    addHexCell(v16Idx, v17Idx, v21Idx, v20Idx, v32Idx, v33Idx, v37Idx, v36Idx);
    addHexCell(v17Idx, v18Idx, v22Idx, v21Idx, v33Idx, v34Idx, v38Idx, v37Idx);
    addHexCell(v18Idx, v19Idx, v23Idx, v22Idx, v34Idx, v35Idx, v39Idx, v38Idx);
    addHexCell(v20Idx, v21Idx, v25Idx, v24Idx, v36Idx, v37Idx, v41Idx, v40Idx);
    addHexCell(v21Idx, v22Idx, v26Idx, v25Idx, v37Idx, v38Idx, v42Idx, v41Idx);
    addHexCell(v22Idx, v23Idx, v27Idx, v26Idx, v38Idx, v39Idx, v43Idx, v42Idx);
    addHexCell(v24Idx, v25Idx, v29Idx, v28Idx, v40Idx, v41Idx, v45Idx, v44Idx);
    addHexCell(v25Idx, v26Idx, v30Idx, v29Idx, v41Idx, v42Idx, v46Idx, v45Idx);
    addHexCell(v26Idx, v27Idx, v31Idx, v30Idx, v42Idx, v43Idx, v47Idx, v46Idx);
    addHexCell(v32Idx, v33Idx, v37Idx, v36Idx, v48Idx, v49Idx, v53Idx, v52Idx);
    addHexCell(v33Idx, v34Idx, v38Idx, v37Idx, v49Idx, v50Idx, v54Idx, v53Idx);
    addHexCell(v34Idx, v35Idx, v39Idx, v38Idx, v50Idx, v51Idx, v55Idx, v54Idx);
    addHexCell(v36Idx, v37Idx, v41Idx, v40Idx, v52Idx, v53Idx, v57Idx, v56Idx);
    addHexCell(v37Idx, v38Idx, v42Idx, v41Idx, v53Idx, v54Idx, v58Idx, v57Idx);
    addHexCell(v38Idx, v39Idx, v43Idx, v42Idx, v54Idx, v55Idx, v59Idx, v58Idx);
    addHexCell(v40Idx, v41Idx, v45Idx, v44Idx, v56Idx, v57Idx, v61Idx, v60Idx);
    addHexCell(v41Idx, v42Idx, v46Idx, v45Idx, v57Idx, v58Idx, v62Idx, v61Idx);
    addHexCell(v42Idx, v43Idx, v47Idx, v46Idx, v58Idx, v59Idx, v63Idx, v62Idx);
}

void Mesh::replaceCellWithTemplate(size_t cIdx, unsigned char Vbitmap, std::vector<size_t> &abandonedCell){
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
    if(Vnum != HEX_SIZE){
        for(size_t idx = 0; idx < HEX_SIZE; idx++)
            localc.push_back(c.at(Global2Local.at(Vbitmap)[idx]));
    }

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
            addCellTemplate(c);
            abandonedCell.push_back(cIdx);
            break;
        default:
            break;
    }
}

void Mesh::refine(std::vector<size_t> &selectedV){
    std::vector<size_t> selectedC;
    std::vector<size_t> abandonedCell;

    /* select cell according to the selected vertexes */
    selectCell(selectedV, selectedC);
    /* remove concavity (nonstandard cell configuration) */
    removeConcavity(selectedC);

    for(auto c:selectedC)
        replaceCellWithTemplate(c, cellInfoMap.at(c).Vbitmap, abandonedCell);

    /* delete all abandoned cells */
    /* has to be deleted from big to small */
    sort(abandonedCell.rbegin(), abandonedCell.rend());
    for(size_t i = 0; i < abandonedCell.size(); i++)
        deleteCell(abandonedCell.at(i));
}