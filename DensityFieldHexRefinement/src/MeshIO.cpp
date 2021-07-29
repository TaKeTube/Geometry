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

#define HEX_SIZE 8

/*
 * meshReader()
 * DESCRIPTION: read mesh from file
 * INPUT: fname - input filenme
 *        mesh - reference to the mesh to be load
 * OUTPUT: mesh
 * RETURN: -1 if fail, 0 if success
 */
int meshReader(const char* fname, Matrix3Xd &V, MatrixXi &C)
{
    string fstring(fname);

    if (fstring.find(".vtk") != fstring.npos)
        vtkReader(fname, V, C);
    else
        return -1;
    return 0;
}

/*
 * vtkReader()
 * DESCRIPTION: read mesh from vtk file
 * INPUT: fname - input filenme
 *        mesh - reference to the mesh to be load
 * OUTPUT: mesh
 * RETURN: none
 */
void vtkReader(const char* fname, Matrix3Xd &V, MatrixXi &C)
{
    /* read vtk file */
    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(fname);
    reader->Update();

    /* read unstructured grid */
    if (reader->IsFileUnstructuredGrid())
    {
        vtkUnstructuredGrid* output = reader->GetUnstructuredGridOutput();
        const vtkIdType vnum = output->GetNumberOfPoints();
        const vtkIdType cnum = output->GetNumberOfCells();
        cout << "UnstructuredGrid: " << vnum << " points " << cnum << " cells" << endl;
        V.resize(3, vnum);

        /* read vertexes */
        double p[3];
        for (vtkIdType i = 0; i < vnum; i++)
        {
            output->GetPoint(i, p);
            V(0, i) = p[0];
            V(1, i) = p[1];
            V(2, i) = p[2];
        }

        /* read cell type */
        const vtkIdType cellType = output->GetCellType(0);
        if (cellType != VTK_HEXAHEDRON){
            std::cout << "Not a Hex mesh" << std::endl;
            return;
        }

        /* read cells */
        C.resize(HEX_SIZE, cnum);
        for (vtkIdType i = 0; i < cnum; i++)
        {
            vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
            output->GetCellPoints(i, idList);
            const vtkIdType csize = idList->GetNumberOfIds();
            for (vtkIdType j = 0; j < csize; j++)
                C(j, i) = idList->GetId(j);
        }
    }
}

/*
 * vtkWriter()
 * DESCRIPTION: write mesh into vtk file
 * INPUT: fname - output filenme
 *        mesh - reference to the mesh to be wrote
 * OUTPUT: vtk file
 * RETURN: none
 */
void vtkWriter(const char* fname, const Matrix3Xd &V, const MatrixXi &C)
{
    const size_t vnum = V.cols();
    const size_t cnum = C.cols();

    ofstream ofs(fname);
    /* write standart format */
    ofs << "# vtk DataFile Version 2.0" << endl
        << fname << endl
        << "ASCII" << endl << endl
        << "DATASET UNSTRUCTURED_GRID" << endl;

    /* write vertexes */
    ofs << "POINTS " << vnum << " float" << endl;
    for (size_t i = 0; i < vnum; i++)
        ofs << fixed << setprecision(7) << V(0, i) << " " << V(1, i) << " " << V(2, i) << endl;

    /* write cellType */
    ofs << "CELLS " << cnum << " ";
    vtkIdType idType = VTK_HEXAHEDRON;
    ofs << 9*cnum << endl;

    /* write cells */
    for (size_t i = 0; i < cnum; i++){
        ofs << HEX_SIZE;
        for (size_t j = 0; j < HEX_SIZE; j++)
            ofs << " " << C(j, i);
        ofs << endl;
    }
    ofs << "CELL_TYPES " << cnum << endl;
    for (size_t i = 0; i < cnum; i++)
        ofs << idType << endl;
}

void vtkWriter(const char* fname, const Matrix3Xd &V, const MatrixXi &C, std::vector<double> Scalar)
{
    vtkWriter(fname, V, C);
    std::ofstream density(fname, std::ios_base::app);
    density << "CELL_DATA " << C.cols() << std::endl
            << "SCALARS scalars float 1" << std::endl
            << "LOOKUP_TABLE default" << std::endl;
    for (size_t i = 0; i < Scalar.size(); i++)
    {
        density << Scalar.at(i) << std::endl;
    }
    density.close();
}

void vtkWriter(const char* fname, const Matrix3Xd &V, const MatrixXi &C, std::vector<int> Scalar)
{
    vtkWriter(fname, V, C);
    std::ofstream density(fname, std::ios_base::app);
    density << "CELL_DATA " << C.cols() << std::endl
            << "SCALARS scalars int 1" << std::endl
            << "LOOKUP_TABLE default" << std::endl;
    for (size_t i = 0; i < Scalar.size(); i++)
    {
        density << Scalar.at(i) << std::endl;
    }
    density.close();
}
