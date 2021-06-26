using namespace std;
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>

#include <vtkPolyData.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkOBJReader.h>
#include <vtkCleanPolyData.h>

#include <eigen3/Eigen/Eigen>

#include "MeshIO.h"

/*
 * vtkWriter()
 * DESCRIPTION: write mesh into vtk file
 * INPUT: fname - output filenme
 *        mesh - reference to the mesh to be wrote
 * OUTPUT: vtk file
 * RETURN: none
 */
void vtkWriter(const char* fname , Eigen::Matrix<float, 3, 27> &P, Eigen::Matrix<int, 8, 8> &C)
{
    const size_t vnum = 27;
    const size_t cnum = 8;

    ofstream ofs(fname);
    /* write standart format */
    ofs << "# vtk DataFile Version 2.0" << endl
        << fname << endl
        << "ASCII" << endl << endl
        << "DATASET UNSTRUCTURED_GRID" << endl;

    /* write vertexes */
    ofs << "POINTS " << vnum << " float" << endl;
    for (size_t i = 0; i < vnum; i++)
        ofs << fixed << setprecision(7) << P(0,i) << " " << P(1,i) << " " << P(2,i) << endl;

    /* write cellType */
    ofs << "CELLS " << cnum << " ";
    vtkIdType idType = VTK_HEXAHEDRON; ofs << 9*cnum << endl;

    /* write cells */
    for (size_t i = 0; i < cnum; i++){
        ofs << 8;
        for (size_t j = 0; j < 8; j++)
            ofs << " " << C(j, i);
        ofs << endl;
    }
    ofs << "CELL_TYPES " << cnum << endl;
    for (size_t i = 0; i < cnum; i++)
        ofs << idType << endl;
}

void vtkWriter(const char* fname , Eigen::Matrix<float, 3, 125> &P, Eigen::Matrix<int, 8, 64> &C)
{
    const size_t vnum = 125;
    const size_t cnum = 64;

    ofstream ofs(fname);
    /* write standart format */
    ofs << "# vtk DataFile Version 2.0" << endl
        << fname << endl
        << "ASCII" << endl << endl
        << "DATASET UNSTRUCTURED_GRID" << endl;

    /* write vertexes */
    ofs << "POINTS " << vnum << " float" << endl;
    for (size_t i = 0; i < vnum; i++)
        ofs << fixed << setprecision(7) << P(0,i) << " " << P(1,i) << " " << P(2,i) << endl;

    /* write cellType */
    ofs << "CELLS " << cnum << " ";
    vtkIdType idType = VTK_HEXAHEDRON; ofs << 9*cnum << endl;

    /* write cells */
    for (size_t i = 0; i < cnum; i++){
        ofs << 8;
        for (size_t j = 0; j < 8; j++)
            ofs << " " << C(j, i);
        ofs << endl;
    }
    ofs << "CELL_TYPES " << cnum << endl;
    for (size_t i = 0; i < cnum; i++)
        ofs << idType << endl;
}