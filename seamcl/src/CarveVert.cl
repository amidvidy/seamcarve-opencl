// Carves a vertical seam from the image
__kernel void carve_vert(__global uchar4* srcImg,
                         __global uchar4* dstImg,
                         __global int *vertSeamPath,
                         int width,
                         int height,
                         int numRowsCarved) {
    int2 myPixel = (int2) (get_global_id(0), get_global_id(1));



    if (myPixel.x < width && myPixel.y < height) {
        int carveIdx = vertSeamPath[myPixel.y];
        if (myPixel.x < carveIdx) {
            dstImg[myPixel.x * width + myPixel.y] = srcImg[myPixel.x * width + myPixel.y];
        } else if (myPixel.x >= carveIdx && myPixel.x < (width - numRowsCarved)) {
            dstImg[(myPixel.x - 1) * width + myPixel.y] = srcImg[myPixel.x * width + myPixel.y];
        } else {
            dstImg[myPixel.x * width + myPixel.y] = (uchar4) (0, 0, 0, 0);
        }
    }
}

