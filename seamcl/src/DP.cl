// Computes cost to reach each pixel from top
// Cumulative cost of a pixel is gradient at that value plus
//   the cost of minimum amongst the three pixels above it (nearby):


// This here is totally unoptimal sketch based on translated code...
//loop x & y (inset) ::: Y[i][j] = G[i][j] + min(min(Y[i-1][j-1], Y[i-1][j]),Y[i-1][j+1]);

// To be launched as a 1 dimensional kernel spanning the "x" dimension only
//   (y iterated internal to kernel)

// This will give way to a better version such as trapezoidal approach, not to mention
//   optimazation and avoidance of bad synchronization and coalescing trouble.

__kernel void dp_path_cost(__read_only float* gradMatrix, // Could become an "image" provided it is gray scale
                             __global float* costMatrix,
                             int inset,
                             int width,
                             int height,
                             int pitch) // pitch can just be width for now
{
// Index into matrix (either row or column major)
#define rM(M,X,Y) (M)[((Y)*pitch+(X))]
#define cM(M,X,Y) (M)[((X)*pitch+(Y))]
// These could become inline functions or otherwise fancier!
    
    // Weights outside the "inset" need to be set reasonably or be unused by
    //   later kernels.  Not sure, but maybe the matrix borders should be
    //   handled nicely by making the input an "image" and utilizing the
    //   pin to edge style of indexing (then allowing x or y to go outside margins.
    
    const int x = get_global_id(0);     // Target x-column
    // if ( inside inset border ) {
        for (int y = inset; y < (height - inset); y++) {
            const float pixelCost = rM(gradMatrix, x, y) + 
                            std::min(   rM(costMatrix,   x, y-1), 
                            std::min(   rM(costMatrix, x-1, y-1),
                                        rM(costMatrix, x+1, y-1) ) );
            
            rM(costMatrix, x, y) = pixelCost;
            //*** Need GLOBAL MEMORY FENCE after each y-row assignment
        }
    //}
}

