// Computes cost to reach each pixel from top
// Cumulative cost of a pixel is gradient at that value plus
//   the cost of minimum amongst the three pixels above it (nearby):


// This here is totally unoptimal sketch based on translated code...
//loop x & y (inset) ::: Y[i][j] = G[i][j] + min(min(Y[i-1][j-1], Y[i-1][j]),Y[i-1][j+1]);

// To be launched as a 1 dimensional kernel spanning the "x" dimension only
//   (y iterated internal to kernel)

// This will give way to a better version such as trapezoidal approach, not to mention
//   optimazation and avoidance of bad synchronization and coalescing trouble.

__kernel void DP_path_cost(__global float* ioMatrix,
                             int inset,
                             int width,
                             int height,
                             int pitch) // pitch can just be width for row-major
{
// Index into matrix (either row or column major)
#define rM(M,X,Y) (M)[((Y)*pitch+(X))]
#define cM(M,X,Y) (M)[((X)*pitch+(Y))]
// These could become inline functions or otherwise fancier!
    
    const int x = get_global_id(0);     // Target x-column
    if ((x < inset) || (x > (width - inset)) { // TODO: Confirm if off by one???
        rM(ioMatrix, x, y) = 0; // TODO: Need actual value
    } else {
        const int bottomInset = height - inset; // TODO: Confirm if off by one???
        int y = 0;
        for (; y < inset; y++) {
            rM(ioMatrix, x, y) = 0; // TODO: Need actual value
            barrier(CLK_GLOBAL_MEM_FENCE);
        }
        for (; y < (height - inset); y++) {
            const float pathCost =  std::min(   rM(ioMatrix,   x, y-1), 
                                    std::min(   rM(ioMatrix, x-1, y-1),
                                                rM(ioMatrix, x+1, y-1) ) );
            rM(ioMatrix, x, y) += pathCost;
            barrier(CLK_GLOBAL_MEM_FENCE);
        }
        for (; y < height; y++) {
            rM(ioMatrix, x, y) = 0; // TODO: Need actual value
            barrier(CLK_GLOBAL_MEM_FENCE);
        }
    }
}

