// Computes cost to reach each pixel from top.
// Cumulative cost of a pixel is gradient at that value plus
//   the cost of minimum amongst the three pixels above it (nearby).
// This will give way to a better version such as trapezoidal approach, not to mention
//   optimazation and avoidance of bad synchronization and coalescing trouble.

// To be launched as a 1 dimensional kernel spanning the "x" dimension only
//   (y iterated internal to kernel)...probably fails across workgroups :(

__kernel void DP_path_cost(
    __global float* ioMatrix, // Gradient IN and PixelCost OUT (in-place)
    int width,      // costMatrix width (matches image width)
    int height,     //   ...and height.
    int pitch       // Distance to advance a pointer into matrix for each row (or col)
) {
// Index into matrix (either row or column major)
#define fmin3(A,B,C) (fmin((A),fmin((B),(C))))
    const int width_m1 = width - 1;
    const int x = get_global_id(0);     // Target x-column
    
    // Can loop this block of code here, striding across X (but staying inside a workgroup)
    {
        __global float *iROW = ioMatrix, *oROW = ioMatrix, *oPREV;
        int y = 0; // Skip zero (leave data alone)
        // iROW[x] = oROW[x];
        while (++y < height) {
            oPREV = oROW, oROW += pitch, iROW += pitch;
            
            const float L = (x < 1) ? MAXFLOAT : oPREV[x-1];
            const float C = oPREV[x];
            const float R = (x < width_m1) ? oPREV[x+1] : MAXFLOAT;
            
            oROW[x] = iROW[x] + fmin3(L, C, R);
            
            barrier(CLK_GLOBAL_MEM_FENCE); // TODO: Does this span workgroups magically???
        }
    }
}
