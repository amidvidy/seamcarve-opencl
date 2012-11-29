// Paints the seam for debugging
__kernel void paint_seam(__write_only image2d_t srcImg,
                         __global int *vertSeamPath,
                         int width,
                         int height) {
    int2 myPixel = (int2) (get_global_id(0), get_global_id(1));

    float4 blue = (float4) (0.0f, 0.0f, 1.0f, 1.0f);
    float4 red = (float4) (1.0f, 0.0f, 0.0f, 1.0f);

    //printf("width = %d, height = %d\n", width, height);
    //printf("hello\n");
    if (myPixel.x < width && myPixel.y < height) {
        if (myPixel.x < vertSeamPath[myPixel.y]) {
            write_imagef(srcImg, myPixel, blue);
        } else {
            write_imagef(srcImg, myPixel, red);
        }
    }
}
