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
            dstImg[myPixel.y * width + myPixel.x] = srcImg[myPixel.y * width + myPixel.x];
        } else if (myPixel.x >= carveIdx && myPixel.x < (width - numRowsCarved)) {
            dstImg[myPixel.y * width + (myPixel.x-1)] = srcImg[myPixel.y * width + myPixel.x];
        } else {
            dstImg[myPixel.y * width + myPixel.x] = (uchar4) (0, 0, 0, 0);
        }
    }
}

