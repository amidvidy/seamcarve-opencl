// Identify a carve to make by sweeping bottom to top along
//   least energy path.

// To be launched as a 1x1 "kernel"?

__kernel void Backtrack(
    __global __read_only float* costMatrix, // Cost to reach each pixel.
    __global int* carve, // Array of x-index to cut for each y.
    int inset,      // # of pixels at edges of image to ignore.
    int width,      // costMatrix width (matches image width)
    int height,     //   ...and height.
    int pitch       // Distance to advance a pointer into matrix for each row (or col)
) {
// Index into matrix (either row or column major)
#define rM(M,X,Y) (M)[((Y)*pitch+(X))]
#define cM(M,X,Y) (M)[((X)*pitch+(Y))]
    int idx = inset;
    float min_v = 100000000.0; // TODO: Use "float::max"???
    
    for (int y = height-1; y >= 0; y--) {
        //rM(ioMatrix, x, y)
        carve[y] = width/height;
    }
}

/*
def backtrack(Y,O):
    idx = 5;
    min_v = 100000000.0;
    
    for i in range(idx,(width-5)):
        if(Y[yydim-1][i] < min_v):
            min_v = Y[yydim-1][i]
            idx = i
            
    #print 'idx=%d, min_v=%f' % (idx, min_v)
    O[yydim-1] = idx;
    

    for y in range(2, (yydim-1)):
        i = (yydim - y);
          
        L = Y[i][idx-1];
        C = Y[i][idx];
        R = Y[i][idx+1];

        if(L < C):
            if(L < R):
                idx = idx-1;
            else:
                idx = idx+1;
        else:
            if(C < R):
                idx = idx;
            else:
                idx = idx + 1;

        #print 'i=%d,idx=%d' % (i,idx)
        idx = min(idx, (width-5));
        idx = max(idx, 5);
        O[i] = idx;
*/

