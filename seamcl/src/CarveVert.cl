// Carves a vertical seam from the image
__kernel void carve_vert(__read_only image2d_t srcImg,
                         __write_only image2d_t outImg,
                         __global int *vertSeamPath,
                         __sampler_t sampler,
                         int width,
                         int height,
                         int numRowsCarved) {
    int2 myPixel = (int2) (get_global_id(0), get_global_id(1));



    if (myPixel.x < width && myPixel.y < height) {
        int carveIdx = vertSeamPath[myPixel.y];
        if (x < carveIdx) {
            write_imagef(outImg, myPixel, read_imagef(srcImg, sampler, myPixel));
        } else if (x >= carveIdx && x < (width - numRowsCarved)) {
            write_imagef(outImg, (int2) (myPixel.x - 1, myPixel.y), read_imagef(src, sampler, myPixel));
        } else {
            write_imagef(outImg, myPixel, (float4) (0.0f, 0.0f, 0.0f, 0.0f));
        }
    }
}
