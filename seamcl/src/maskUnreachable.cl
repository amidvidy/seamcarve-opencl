__kernel void mask_unreachable(__global float *energyMatrix,
                                  int width,
                                  int height,
                                  int pitch,
                                  int colsRemoved) {
#define rM(M,X,Y) (M)[((Y)*pitch+(X))]
#define RADIUS 1 // mask off gaussian values that have been tainted by garbage
    int2 myCell = (int2) (get_global_id(0), get_global_id(1));

    if (myCell.x < RADIUS) {
        rM(energyMatrix, myCell.x, myCell.y) = MAXFLOAT;
    } else if (myCell.x < width && myCell.x > (width - (RADIUS + colsRemoved))) {
        rM(energyMatrix, myCell.x, myCell.y) = MAXFLOAT;
    }
}

