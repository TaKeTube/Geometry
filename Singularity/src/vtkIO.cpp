#include <vtkSmartPointer.h>
#include <vtkPoints.h> 
#include <vtkPolygon.h>
#include <vtkLine.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "vtkIO.hpp"
#include "Singularity.hpp"

using namespace std;

void vtkSingularitiesWriter(OpenVolumeMesh::GeometricPolyhedralMeshV3f &mMesh,
    std::vector<Edge> &singularity, const char* fname)
{
    vtkSmartPointer<vtkPoints> sVertex = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPolygon> sPolygon = vtkSmartPointer<vtkPolygon>::New();
    vtkSmartPointer<vtkLine> sLine = vtkSmartPointer<vtkLine>::New();

    sPolygon->GetPointIds()->SetNumberOfIds(2*singularity.size());

    for(size_t i = 0; i < singularity.size(); i++){
        Edge e = singularity.at(i);
        Vec3f v1 = mMesh.vertex(e.from_vertex());
        Vec3f v2 = mMesh.vertex(e.from_vertex());

        sVertex->InsertNextPoint(v1.data()[0], v1.data()[1], v1.data()[2]);
        sVertex->InsertNextPoint(v2.data()[0], v2.data()[1], v2.data()[2]);
        
        sPolygon->GetPointIds()->SetId(2*i, 2*i);
        sPolygon->GetPointIds()->SetId(2*i+1, 2*i+1);

        sLine->GetPointIds()->SetId(2*i, 2*i+1);
        std::cout<< " " << i << " ";
    }

    vtkSmartPointer<vtkCellArray> sCells = vtkSmartPointer<vtkCellArray>::New();
    sCells->InsertNextCell(sPolygon);
    sCells->InsertNextCell(sLine);
    vtkSmartPointer<vtkPolyData> sPolygonPolyData = vtkSmartPointer<vtkPolyData>::New();
    sPolygonPolyData->SetPoints(sVertex);
    sPolygonPolyData->SetPolys(sCells);

    /* clean polydata, merge repetitive points */
    vtkSmartPointer<vtkCleanPolyData> cleanFilter = vtkSmartPointer<vtkCleanPolyData>::New();
    cleanFilter->SetInputData(sPolygonPolyData);
    cleanFilter->Update();
    vtkPolyData* output = cleanFilter->GetOutput();

    vtkSmartPointer<vtkPolyDataWriter> vtkWriter = vtkSmartPointer<vtkPolyDataWriter>::New();
    vtkWriter->SetInputData(output);
    vtkWriter->SetFileName(fname);
    vtkWriter->Write();
}