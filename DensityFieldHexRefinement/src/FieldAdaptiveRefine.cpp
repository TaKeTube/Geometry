#include "FieldAdaptiveRefine.h"
#include "Utility.hpp"

#define MAX_ITER_NUM 20

double EvalDensity(const Matrix3Xd &V, const VectorXi &c, const std::function<double(Vector3d)> &DensityField){
    Vector3d v0 = V.col(c(0)), v1 = V.col(c(1)), v2 = V.col(c(2)), v3 = V.col(c(3)),
             v4 = V.col(c(4)), v5 = V.col(c(5)), v6 = V.col(c(6)), v7 = V.col(c(7));
    return (DensityField(v0)+DensityField(v1)+DensityField(v2)+DensityField(v3)+
            DensityField(v4)+DensityField(v5)+DensityField(v6)+DensityField(v7))*0.125;
}

double EvalDensity(const std::vector<Vector3d> &V, const std::function<double(Vector3d)> &DensityField){
    return (DensityField(V.at(0))+DensityField(V.at(1))+DensityField(V.at(2))+DensityField(V.at(3))+
            DensityField(V.at(4))+DensityField(V.at(5))+DensityField(V.at(6))+DensityField(V.at(7)))*0.125;
}

int FieldAdaptiveRefine(Matrix3Xd &V, MatrixXi &C, const std::function<double(Vector3d)> &DensityField){
    int IterCount = 0;
    std::queue<int> TargetC;
    if(MarkTargetHex(V, C, TargetC, DensityField) == -1) 
        return -1;

    while(!TargetC.empty() || IterCount < MAX_ITER_NUM){
        if(RefineTargetHex(V, C, TargetC) == -1)
            return -1;
        if(MarkTargetHex(V, C, TargetC, DensityField) == -1)
            return -1;
    }

    EvalFieldAdaptiveMesh(V, C, DensityField);
    return 0;
}

int MarkTargetHex(const Matrix3Xd &V, const MatrixXi &C, std::queue<int> &TargetC, const std::function<double(Vector3d)> &DensityField){
    for(size_t i; i < C.cols(); i++){
        MatrixXi c = C.col(i);
        if(EvalDensity(V, c, DensityField) > 1/HexVolume(V, c)){
            TargetC.push(i);
        }
    }
}

int RefineTargetHex(Matrix3Xd &V, MatrixXi &C, std::queue<int> &TargetC);

int EvalFieldAdaptiveMesh(const Matrix3Xd &V, const MatrixXi &C, const std::function<double(Vector3d)> &DensityField);
