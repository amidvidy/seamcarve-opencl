// Does Dynamic Programming with Trapezoid blocking
// Trapezoid height and width (at top) need to be specified.

__kernel void DP_trapezoid(
    __global float* ioMatrix, // Gradient IN and PixelCost OUT (in-place)
    int width,      // costMatrix width (matches image width)
    int height,     //   ...and height.
    int pitch       // Distance to advance a pointer into matrix for each row (or col)
    int colsRemoved // number of seams already carved.
    int trapz_height // The height of trapezoid.
) {

// Index into matrix
#define rM(M,X,Y) (M)[((Y)*pitch+(X))]
    const int imgWidth = width - colsRemoved;
    const int localIdx = get_local_id(0);
    const int localSize = get_local_size(0);
    const int blockSize = imgWidth / localSize;
    const int startX = localIdx * blockSize; // left top x-coor of trapezoid.
    const int endX = (localIdx == localSize - 1) ? imgEndIdx : startX + blockSize; // right top x-coor of trapezoid.
    // width of current level of the trapezoid.
    for (int y_block = 1; y_block < height; y_block+=trapz_height) {

      // a new trapezoid:
      int trap_x_start = max(startX - trapz_height + 1, 0);
      // left border of this row of the trapezoid.
      int trap_x_end = min(endX + trapz_height - 1, imgWidth);
      // right border of this trapezoid row.
	for (int y = y_block; y < y_block + trapz_height;++y) {
	  for (int x = trap_x_start; x < trap_x_end; ++x) {
	    const float pathCost = fmin(rM(ioMatrix, max(x-1, 0),     y - 1),
                                   fmin(rM(ioMatrix,           x,     y - 1),
                                        rM(ioMatrix, min(x+1, imgEndIdx), y - 1)));
            rM(ioMatrix, x, y) += pathCost;
	    }
        }
        // next level of trapezoid is skinnier:
	trapezoid_x_start++;
	trapezoid_x_end--; 
	barrier(CLK_GLOBAL_MEM_FENCE);
    }
}
