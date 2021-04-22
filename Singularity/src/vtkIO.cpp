#define USING_VTK   0

#if USING_VTK
    #include <vtkSmartPointer.h>
    #include <vtkPoints.h> 
    #include <vtkLine.h>
    #include <vtkPolyDataWriter.h>
    #include <vtkPolyData.h>
    #include <vtkPointData.h>
    #include <vtkCellData.h>
    #include <vtkCleanPolyData.h>
#endif

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

#include "vtkIO.hpp"
#include "Singularity.hpp"

using namespace std;

#if USING_VTK

void vtkSingularitiesWriter(OpenVolumeMesh::GeometricPolyhedralMeshV3f &mMesh,
    std::vector<Edge> &singularity, const char* fname)
{
    vtkSmartPointer<vtkPoints> sVertex = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> sCells = vtkSmartPointer<vtkCellArray>::New();

    for(size_t i = 0; i < singularity.size(); i++){
        Edge e = singularity.at(i);
        Vec3f v1 = mMesh.vertex(e.from_vertex());
        Vec3f v2 = mMesh.vertex(e.to_vertex());
        vtkSmartPointer<vtkLine> sLine = vtkSmartPointer<vtkLine>::New();

        sVertex->InsertNextPoint(v1.data()[0], v1.data()[1], v1.data()[2]);
        sVertex->InsertNextPoint(v2.data()[0], v2.data()[1], v2.data()[2]);

        sLine->GetPointIds()->SetId(0, 2*i);
        sLine->GetPointIds()->SetId(1, 2*i+1);
        sCells->InsertNextCell(sLine);
    }

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
    // vtkWriter->SetInputData(sPolygonPolyData);
    vtkWriter->SetFileName(fname);
    vtkWriter->Write();
}

#else

void vtkSingularitiesWriter(OpenVolumeMesh::GeometricPolyhedralMeshV3f &mMesh,
    std::vector<Edge> &singularity, const char* fname)
{
    const size_t vNum = mMesh.n_vertices();
    const size_t sEdgeNum = singularity.size();
    const size_t sVertexNum = 2*sEdgeNum;

    std::ofstream ofs(fname);
    ofs << "# vtk DataFile Version 2.0" << endl
        << fname << endl
        << "ASCII" << endl << endl
        << "DATASET POLYDATA" << endl;
    ofs << "POINTS " << vNum << " float" << endl;
    for (auto v_iter = mMesh.vertices_begin(); v_iter != mMesh.vertices_end(); ++v_iter){
        Vec3f v = mMesh.vertex(v_iter.cur_handle());
        ofs << std::fixed << setprecision(7) << v.data()[0] << " " << v.data()[1] << " " << v.data()[2] << endl;
    }

    // ofs << "VERTICES " << sVertexNum << " " << 2 * sVertexNum << endl;
    // for(size_t i = 0; i < singularity.size(); i++){
    //     Edge e = singularity.at(i);
    //     int v1Idx = e.from_vertex().idx();
    //     int v2Idx = e.to_vertex().idx();
    //     ofs << "1 " << v1Idx << std::endl;
    //     ofs << "1 " << v2Idx << std::endl;
    // }

    ofs << "LINES " << sEdgeNum << " " << 3 * sEdgeNum << endl;
    for(size_t i = 0; i < singularity.size(); i++){
        Edge e = singularity.at(i);
        int v1Idx = e.from_vertex().idx();
        int v2Idx = e.to_vertex().idx();
        ofs << "2 " << v1Idx << " " << v2Idx << std::endl;
    }
}

#endif