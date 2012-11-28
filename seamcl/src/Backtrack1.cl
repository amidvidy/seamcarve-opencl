// Identify a carveArray to make by sweeping bottom to top along
//   least energy path.

// To be launched as a 1x1 "kernel"?

__kernel void Backtrack(
    __global float* costMatrix, // Cost to reach each pixel.
    __global int* carveArray, // Array of x-index to cut for each y.
    int width,      // costMatrix width (matches image width)
    int height,     //   ...and height.
    int pitch       // Distance to advance a pointer into matrix for each row (or col)
) {
// Index into matrix (either row or column major)
#define pROW(M,Y) ((M)+((Y)*pitch))
    const int width_m1 = width-1;
    __global float *ROW; // Points to base of each matrix row
    int y = height-1; // The row being contemplated (starts at bottom)
    int carveX = 0; // The currently contemplated carve-x on each row
    
    ROW = pROW(costMatrix, y);
    int x = carveX;
    float min_v = ROW[x]; // Could also just start with row[carveX] value
    while (++x < width) {
        if (ROW[x] < min_v) {
            min_v = ROW[x]; // Keep track of min, though we just care about
            carveX = x;     //   which index holds the min.
        }
    }
    carveArray[y] = carveX;
    // Continue carve upward to top
    while (y >= 0) {
        ROW = pROW(costMatrix, y);
        const float L = (carveX < 1) ? MAXFLOAT : ROW[carveX-1];
        const float C = ROW[carveX];
        const float R = (carveX >= width_m1) ? MAXFLOAT : ROW[carveX+1];
        if (L < C) { // Backtrack along minimal energy between three competitors
            carveX += (L < R) ? -1 : 1;
        } else {
            carveX += (C < R) ?  0 : 1;
        }
        carveArray[y] = carveX;
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

