__kernel void backtrace_vert(__global float *energyMatrix,
                             __global int *vertSeamPath,
                             __global int *startMinIdx,
                             int width,
                             int height,
                             int pitch) {
// Index into matrix
#define rM(M,X,Y) (M)[((Y)*pitch+(X))]

    const int localIdx = get_local_id(0);
    const int startRowIdx = height - 2;

    int curIdx = *startMinIdx;
    float energyMin;

    if (localIdx == 0) {
        // We know the first value already
        vertSeamPath[height - 1] = curIdx;

        // Backtrack
        for (int y = startRowIdx; y >= 0; y--) {

            float left = (curIdx < 0) ? MAXFLOAT : rM(energyMatrix, curIdx - 1, y);
            float center = rM(energyMatrix, curIdx, y);
            float right (curIdx > width) ? MAXFLOAT : rM(energyMatrix, curIdx + 1, y);

            if (left < center) {
                curIdx += (left < right) -1 : 1;
            } else {
                curIdx += (center < right) 0 : 1;
            }

            vertSeamPath[y] = curIdx;
        }
    }
}
