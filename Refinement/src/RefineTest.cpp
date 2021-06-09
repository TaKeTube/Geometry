#include <string>
#include <sstream>

#include "MeshIO.h"
#include "Mesh.h"
#include "global.hpp"
#include "RefineTest.h"

// #define TEST_1_VERT
// #define TEST_2_VERT
// #define TEST_3_VERT
// #define TEST_4_VERT
#define TEST_5_VERT

void refine1vertTest(int i, char* test_file){
    std::string idx;
    Mesh mesh = Mesh();

    /* select target vertex */
    std::vector<size_t> selectedV;
    selectedV.push_back(i);
    
    /* read mesh */
    meshReader(test_file, mesh);
    
    /* get vertex - cell pairs */
    mesh.getVI_CI();
    
    /* detect number of flat angles then print it out */
    mesh.refine(selectedV);
    
    /* output the processed mesh */
    idx = '0'+(char)i;
    vtkWriter((std::string("v")+idx+std::string(".vtk")).c_str(), mesh);
}

void refine2vertTest(int i, int j, char* test_file){
    Mesh mesh = Mesh();
    std::string iIdx, jIdx;

    /* select 2 target vertexes */
    std::vector<size_t> selectedV;
    selectedV.push_back(i);
    selectedV.push_back(j);
    
    /* read mesh */
    meshReader(test_file, mesh);
    
    /* get vertex - cell pairs */
    mesh.getVI_CI();
    
    /* detect number of flat angles then print it out */
    mesh.refine(selectedV);
    
    /* output the processed mesh */
    iIdx = '0'+(char)i;
    jIdx = '0'+(char)j;
    vtkWriter((std::string("v")+iIdx+jIdx+std::string(".vtk")).c_str(), mesh);
}

void refine3vertTest(int i, int j, int k, char* test_file){
    /* test 3 vertexes selected */
    Mesh mesh = Mesh();
    std::string iIdx, jIdx, kIdx;

    /* select 2 target vertexes */
    std::vector<size_t> selectedV;
    selectedV.push_back(i);
    selectedV.push_back(j);
    selectedV.push_back(k);
    
    /* read mesh */
    meshReader(test_file, mesh);
    
    /* get vertex - cell pairs */
    mesh.getVI_CI();
    
    /* detect number of flat angles then print it out */
    mesh.refine(selectedV);
    
    /* output the processed mesh */
    iIdx = '0'+(char)i;
    jIdx = '0'+(char)j;
    kIdx = '0'+(char)k;
    vtkWriter((std::string("v")+iIdx+jIdx+kIdx+std::string(".vtk")).c_str(), mesh);
}

void refine4vertTest(int i, int j, int k, int n, char* test_file){
    /* test 3 vertexes selected */
    Mesh mesh = Mesh();
    std::string iIdx, jIdx, kIdx, nIdx;

    /* select 2 target vertexes */
    std::vector<size_t> selectedV;
    selectedV.push_back(i);
    selectedV.push_back(j);
    selectedV.push_back(k);
    selectedV.push_back(n);

    /* read mesh */
    meshReader(test_file, mesh);
    
    /* get vertex - cell pairs */
    mesh.getVI_CI();
    
    /* detect number of flat angles then print it out */
    mesh.refine(selectedV);
    
    /* output the processed mesh */
    iIdx = '0'+(char)i;
    jIdx = '0'+(char)j;
    kIdx = '0'+(char)k;
    nIdx = '0'+(char)n;
    vtkWriter((std::string("v")+iIdx+jIdx+kIdx+nIdx+std::string(".vtk")).c_str(), mesh);
}

void refine5vertTest(int i, int j, int k, int n, int m, char* test_file){
    /* test 3 vertexes selected */
    Mesh mesh = Mesh();
    std::string iIdx, jIdx, kIdx, nIdx, mIdx;

    /* select 2 target vertexes */
    std::vector<size_t> selectedV;
    selectedV.push_back(i);
    selectedV.push_back(j);
    selectedV.push_back(k);
    selectedV.push_back(n);
    selectedV.push_back(m);

    /* read mesh */
    meshReader(test_file, mesh);
    
    /* get vertex - cell pairs */
    mesh.getVI_CI();
    
    /* detect number of flat angles then print it out */
    mesh.refine(selectedV);
    
    /* output the processed mesh */
    iIdx = '0'+(char)i;
    jIdx = '0'+(char)j;
    kIdx = '0'+(char)k;
    nIdx = '0'+(char)n;
    mIdx = '0'+(char)m;
    vtkWriter((std::string("v")+iIdx+jIdx+kIdx+nIdx+mIdx+std::string(".vtk")).c_str(), mesh);
}

void runTest(){
    char test_file[] = "../data/cube.vtk";

    #ifdef TEST_1_VERT
    /* test 1 vertex selected */
    for(int i = 0; i < HEX_SIZE; i++)
        refine1vertTest(i, test_file);
    #endif

    #ifdef TEST_2_VERT
    /* test 2 vertexes selected */
    for(int i = 0; i < HEX_SIZE; i++)
        for(int j = i+1; j < HEX_SIZE; j++)
            refine2vertTest(i, j, test_file);
    #endif

    #ifdef TEST_3_VERT
    /* test 3 vertexes selected */
    refine3vertTest(0, 1, 2, test_file);
    refine3vertTest(4, 5, 6, test_file);
    refine3vertTest(1, 2, 4, test_file);
    #endif

    #ifdef TEST_4_VERT
    /* test 4 vertexes selected */
    refine4vertTest(0, 1, 2, 3, test_file);
    refine4vertTest(4, 5, 6, 7, test_file);
    refine4vertTest(1, 2, 4, 6, test_file);
    #endif

    #ifdef TEST_5_VERT
    /* test 5 vertexes selected */
    refine5vertTest(0, 1, 2, 3, 4, test_file);
    #endif
}