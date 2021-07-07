#include "FieldAdaptiveRefine.h"

#define MAX_ITER_NUM 20

int FieldAdaptiveRefine(Matrix3Xd &V, MatrixXi &C, std::function<Vector3d> DensityField){
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

int MarkTargetHex(const Matrix3Xd &V, const MatrixXi &C, std::queue<int> &TargetC, std::function<Vector3d> DensityField){

}

int RefineTargetHex(Matrix3Xd &V, MatrixXi &C, std::queue<int> &TargetC);

int EvalFieldAdaptiveMesh(Matrix3Xd &V, MatrixXi &C, std::function<Vector3d> DensityField);