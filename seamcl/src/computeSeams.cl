

// Computes cost to reach each pixel from top.
__kernel void computeSeams(__global float *ioMatrix,
                           int width,
                           int height,
                           int pitch) {

// Index into matrix
#define rM(M,X,Y) (M)[((Y)*pitch+(X))]

    const int localIdx = get_local_id(0);
    const int localSize = get_local_size(0);
    const int blockSize = width / get_local_size(0);
    const int startX = localIdx * blockSize;
    const int endX = (localIdx == localSize - 1) ? width : startX + blockSize;

    for (int y = 1; y < height; ++y) {
        for (int x = startX; x < endX; ++x) {
            const float pathCost = fmin(rM(ioMatrix, max(x-1, 0),     y - 1),
                                   fmin(rM(ioMatrix,           x,     y - 1),
                                        rM(ioMatrix, min(x+1, width), y - 1)));
            rM(ioMatrix, x, y) += pathCost;
        }
        barrier(CLK_GLOBAL_MEM_FENCE);
    }

}
