// Identify a carveArray to make by sweeping bottom to top along
//   least energy path.

// To be launched as a 1x1 "kernel"?

__kernel void Backtrack(
    __constant float* costMatrix, // Cost to reach each pixel.
    __global int* carveArray, // Array of x-index to cut for each y.
    int width,      // costMatrix width (matches image width)
    int height,     //   ...and height.
    int pitch       // Distance to advance a pointer into matrix for each row (or col)
) {
// Index into matrix (either row or column major)
#define pROW(M,Y) (M)+((Y)*pitch)
    int inset=0;
    __constant float *ROW; // Points to base of each matrix row
    int y = height-inset-1; // The row being contemplated
    int carveX = inset; // The currently contemplated carve-x on each row
    
    ROW = pROW(costMatrix, y);
    int x = carveX;
    float min_v = ROW[x]; // Could also just start with row[carveX] value
    while (++x < (width-inset)) {
        if (ROW[x] < min_v) {
            min_v = ROW[x]; // Keep track of min, though we just care about
            carveX = x;     //   which index holds the min.
        }
    }
    // Repeat first carve through bottom inset-border
    for (int tempY = height-1; tempY >= y; y--) {
        carveArray[y] = carveX;
    }
    // Repeat last carve through top inset-border
    while (y > inset) {
        ROW = pROW(costMatrix, y);
        const float L = ROW[carveX-1], C = ROW[carveX], R = ROW[carveX+1]; // TODO: grab these with fancy 3-fer fetch
        if (L < C) { // Backtrack along minimal energy between three competitors
            carveX += (L < R) ? -1 : 1;
        } else {
            carveX += (C < R) ?  0 : 1;
        }
        // Clip to left & right inset-borders
        if (carveX > (width - inset)) carveX = (width - inset);
        if (carveX < inset) carveX = inset;
        carveArray[y] = carveX;
    }
    // Repeat last carve through top inset-border
    while (y > 0) {
        carveArray[y--] = carveX;
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

