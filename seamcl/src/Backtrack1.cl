// Identify a carve to make by sweeping bottom to top along
//   least energy path.

// To be launched as a 1 dimensional kernel spanning the "x" dimension only
//   (y iterated internal to kernel)

__kernel void Backtrack(__read_only float* costMatrix,
                             __global float* carve,
                             int width,
                             int height,
                             int pitch) // pitch can just be width for row-major
{
// Index into matrix (either row or column major)
#define rM(M,X,Y) (M)[((Y)*pitch+(X))]
#define cM(M,X,Y) (M)[((X)*pitch+(Y))]
    
    for (int y = height-1; y >= 0; y--) {
        //rM(ioMatrix, x, y)
        carve[y] = width/height;
    }
}

