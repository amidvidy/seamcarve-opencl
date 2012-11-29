// This only works with one workgroup! If this is the slowest part, we can separate out the reduction
// later.
__kernel void find_min_vert(__global float *energyMatrix,
                            __global float *outMin,
                            __global int *outMinIdx,
                            __local float *reductionMemIdx,
                            __local float *reductionMemEnergy,
                            int width,
                            int height,
                            int pitch,
                            int colsRemoved) {
// Index into matrix
#define rM(M,X,Y) (M)[((Y)*pitch+(X))]

    const int localIdx = get_local_id(0);
    const int localSize = get_local_size(0);

    const int lastRowIdx = height - 1;
    int energyMinIdx = 0;
    float energyMin = MAXFLOAT;

    // Find local min
    for (int curIdx = localIdx; curIdx < (width-colsRemoved); curIdx+= localSize) {
        float curEnergy = rM(energyMatrix, curIdx, lastRowIdx);

        if (curEnergy < energyMin) {
            energyMin = curEnergy;
            energyMinIdx = curIdx;
        }
    }

    reductionMemIdx[localIdx] = energyMinIdx;
    reductionMemEnergy[localIdx] = energyMin;

    // Local memory reduction
    for (uint reductionIdx = localSize/2; reductionIdx > 0; reductionIdx >>=1) {
        barrier(CLK_LOCAL_MEM_FENCE);
        if (localIdx < reductionIdx) {

            float myEnergy = reductionMemEnergy[localIdx];
            float reduceEnergy = reductionMemEnergy[localIdx + reductionIdx];
            if (reduceEnergy < myEnergy) {
                reductionMemEnergy[localIdx] = reduceEnergy;
                reductionMemIdx[localIdx] = reductionMemIdx[localIdx + reductionIdx];
            }
        }
    }

    if (localIdx == 0) {
        *outMin = reductionMemEnergy[0];
        *outMinIdx = reductionMemIdx[0];
    }

}
