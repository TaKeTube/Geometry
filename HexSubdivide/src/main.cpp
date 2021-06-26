#include <iostream>
#include "subdiv.h"
#include "MeshIO.h"

int main(){
    Eigen::Matrix<float, 3, 8> P;
    Eigen::Matrix<float, 3, 27> NewP1To8;
    Eigen::Matrix<float, 3, 125> NewP1To64;
    Eigen::Matrix<int, 8, 8> C1To8;
    Eigen::Matrix<int, 8, 64> C1To64;

    P << 0, 0, 0, 0, 2, 2, 2, 1,
         0, 0, 2, 2, 0, 0, 2, 2,
         0, 2, 2, 0, 0, 2, 2, 0;

    HexDiv1To8(P, NewP1To8, C1To8);
    HexDiv1To64(P, NewP1To64, C1To64);

    for(int CIdx = 0; CIdx < 8; CIdx++){
        for(int PIdx = 0; PIdx < 8; PIdx++)
            std::cout << NewP1To8.col(C1To8(PIdx, CIdx)).transpose() << std::endl;
        std::cout<<"======="<<std::endl;
    }
    vtkWriter("1to8.vtk", NewP1To8, C1To8);

    for(int CIdx = 0; CIdx < 64; CIdx++){
        for(int PIdx = 0; PIdx < 8; PIdx++)
            std::cout << NewP1To64.col(C1To64(PIdx, CIdx)).transpose() << std::endl;
        std::cout<<"======="<<std::endl;
    }
    vtkWriter("1to64.vtk", NewP1To64, C1To64);
}