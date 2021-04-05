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
#include <string.h>

#include <vtkPolyData.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>

#include "MeshIO.h"
#include "Mesh.h"

void vtkReader(const char* fname , Mesh& mesh)
{
    // Get all data from the file
    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(fname);
    reader->Update();

    // All of the standard data types can be checked and obtained like this:
    if (reader->IsFilePolyData())
    {
        vtkPolyData* output = reader->GetPolyDataOutput();
        const vtkIdType vnum = output->GetNumberOfPoints();
        const vtkIdType cnum = output->GetNumberOfPolys();
        std::cout << "PolyData: " << vnum << " points " << cnum << " polys" << std::endl;
        std::vector<Vertex>& V = mesh.V;
        V.resize(vnum);
        double p[3];
        for (vtkIdType i = 0; i < vnum; i++)
        {
            output->GetPoint(i, p);
            V.at(i).x = p[0];
            V.at(i).y = p[1];
            V.at(i).z = p[2];
        }
        mesh.cellType = POLYGON;
        std::vector<Cell>& C = mesh.C;
        for (vtkIdType i = 0; i < cnum; i++)
        {
            vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
            output->GetPolys()->GetCell(i, idList);
            const vtkIdType csize = idList->GetNumberOfIds();
            Cell c(csize);
            for (vtkIdType j = 0; j < csize; j++)
                c.at(j) = idList->GetId(j);
            C.push_back(c);
        }
        C.resize(C.size());
    }
    else if (reader->IsFileUnstructuredGrid())
    {
        vtkUnstructuredGrid* output = reader->GetUnstructuredGridOutput();
        const vtkIdType vnum = output->GetNumberOfPoints();
        const vtkIdType cnum = output->GetNumberOfCells();
        std::cout << "UnstructuredGrid: " << vnum << " points " << cnum << " cells" << std::endl;
        std::vector<Vertex>& V = mesh.V;
        V.resize(vnum);
        // Read V
        double p[3];
        for (vtkIdType i = 0; i < vnum; i++)
        {
            output->GetPoint(i, p);
            V.at(i).x = p[0];
            V.at(i).y = p[1];
            V.at(i).z = p[2];
        }
        // Read CellType
        const vtkIdType cellType = output->GetCellType(0);
        if (cellType == VTK_TRIANGLE) mesh.cellType = TRIANGLE;
        else if (cellType == VTK_QUAD) mesh.cellType = QUAD;
        else if (cellType == VTK_TETRA) mesh.cellType = TETRAHEDRA;
        else if (cellType == VTK_HEXAHEDRON) mesh.cellType = HEXAHEDRA;
        // Read C
        std::vector<Cell>& C = mesh.C;
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