// Computes cost to reach each pixel from top

// This is totally unoptimal sketch based on translated code...

__kernel void dp_path_cost(__read_only float* gradMatrix,
                             __global float* costMatrix,
                             int inset,
                             int width,
                             int height)
{
    const int x = get_global_id(0);     // Target x-column
    // if ( inside inset border ) {
        for (int y = inset; y < (height - inset); y++) {
            // Cumulative cost of a pixel is gradient at that value plus
            //   the cost of three pixels above it (nearby):
            costMatrix[x][y] = gradMatrix[x][y] + 
                std::min( costMatrix[x][y], 
                    std::min( costMatrix[x-1][y], costMatrix[x][y])
                );
            //GLOBAL MEMORY FENCE for each y-row since depends on row above
        }
    //}
}

