/* Mesh IO
 * Include basic IO function of mesh file
 * This file is a modified version of open source code MeshFileReader.cpp in
 *      CotrikMeshing
 *      Address: https://github.com/Cotrik/CotrikMeshing
 *      Author: Cotrik
 * Modified by TaKeTube
 */

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

#include "MeshIO.h"

/*
 * meshReader()
 * DESCRIPTION: read mesh from file
 * INPUT: fname - input filenme
 *        mesh - reference to the mesh to be load
 * OUTPUT: mesh
 * RETURN: -1 if fail, 0 if success
 */
int meshReader(const char* fname, Mesh& mesh)
{
    string fstring(fname);

    if (fstring.find(".vtk") != fstring.npos)
        vtkReader(fname, mesh);
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
void vtkReader(const char* fname , Mesh& mesh)
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
        Vertexes& V = mesh.V;
        V.resize(vnum);

        /* read vertexes */
        double p[3];
        for (vtkIdType i = 0; i < vnum; i++)
        {
            output->GetPoint(i, p);
            V.at(i).x() = p[0];
            V.at(i).y() = p[1];
            V.at(i).z() = p[2];
        }

        /* read cell type */
        const vtkIdType cellType = output->GetCellType(0);
        if (cellType == VTK_TRIANGLE) mesh.cellType = TRIANGLE;
        else if (cellType == VTK_QUAD) mesh.cellType = QUAD;
        else if (cellType == VTK_TETRA) mesh.cellType = TETRAHEDRA;
        else if (cellType == VTK_HEXAHEDRON) mesh.cellType = HEXAHEDRA;

        /* read cells */
        vector<Cell>& C = mesh.C;
        for (vtkIdType i = 0; i < cnum; i++)
        {
            vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
            output->GetCellPoints(i, idList);
            const vtkIdType csize = idList->GetNumberOfIds();
            Cell c(csize);
            for (vtkIdType j = 0; j < csize; j++)
                c.at(j) = idList->GetId(j);
            C.push_back(c);
        }
        C.resize(C.size());
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
void vtkWriter(const char* fname , Mesh& mesh)
{
    const Vertexes V = mesh.V;
    const vector<Cell>& C = mesh.C;
    const size_t vnum = V.size();
    const size_t cnum = C.size();

    ofstream ofs(fname);
    /* write standart format */
    ofs << "# vtk DataFile Version 2.0" << endl
        << fname << endl
        << "ASCII" << endl << endl
        << "DATASET UNSTRUCTURED_GRID" << endl;

    /* write vertexes */
    ofs << "POINTS " << vnum << " float" << endl;
    for (size_t i = 0; i < vnum; i++)
        ofs << fixed << setprecision(7) << V.at(i).x() << " " << V.at(i).y() << " " << V.at(i).z() << endl;

    /* write cellType */
    ofs << "CELLS " << cnum << " ";
    vtkIdType idType = VTK_TRIANGLE;
    if (mesh.cellType == TRIANGLE) ofs << 4*cnum << endl;
    else if (mesh.cellType == QUAD) {idType = VTK_QUAD;  ofs << 5*cnum << endl;}
    else if (mesh.cellType == TETRAHEDRA) {idType = VTK_TETRA; ofs << 5*cnum << endl;}
    else if (mesh.cellType == HEXAHEDRA) {idType = VTK_HEXAHEDRON; ofs << 9*cnum << endl;}

    /* write cells */
    for (size_t i = 0; i < cnum; i++){
        ofs << C.at(i).size();
        for (size_t j = 0; j < C.at(i).size(); j++)
            ofs << " " << C.at(i).at(j);
        ofs << endl;
    }
    ofs << "CELL_TYPES " << cnum << endl;
    for (size_t i = 0; i < cnum; i++)
        ofs << idType << endl;
}