
// Gaussian filter of image

__kernel void gaussian_filter(__read_only image2d_t srcImg,
                              __write_only image2d_t dstImg,
                              sampler_t sampler,
                              int width, int height)
{
    // Laplacian Gaussian Kernel is:

    float kernelWeights[81] = {0.0f, 1.0f, 1.0f, 2.0f,   2.0f,   2.0f,   1.0f, 1.0f, 0.0f,
                               1.0f, 2.0f, 4.0f, 5.0f,   5.0f,   5.0f,   4.0f, 2.0f, 1.0f,
                               1.0f, 4.0f, 5.0f, 3.0f,   0.0f,   3.0f,   5.0f, 4.0f, 1.0f,
                               2.0f, 5.0f, 3.0f, -12.0f, -24.0f, -12.0f, 3.0f, 5.0f, 2.0f,
                               2.0f, 5.0f, 0.0f, -24.0f, -40.0f, -24.0f, 0.0f, 5.0f, 2.0f,
                               2.0f, 5.0f, 3.0f, -12.0f, -24.0f, -12.0f, 3.0f, 5.0f, 2.0f,
                               1.0f, 4.0f, 5.0f, 3.0f,   0.0f,   3.0f,   5.0f, 4.0f, 1.0f,
                               1.0f, 2.0f, 4.0f, 5.0f,   5.0f,   5.0f,   4.0f, 2.0f, 1.0f,
                               0.0f, 1.0f, 1.0f, 2.0f,   2.0f,   2.0f,   1.0f, 1.0f, 0.0f};

    int2 startImageCoord = (int2) (get_global_id(0) - 4, get_global_id(1) - 4);
    int2 endImageCoord   = (int2) (get_global_id(0) + 4, get_global_id(1) + 4);
    int2 outImageCoord = (int2) (get_global_id(0), get_global_id(1));

    if (outImageCoord.x < width && outImageCoord.y < height)
    {
        int weight = 0;
        float4 outColor = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
        for( int y = startImageCoord.y; y <= endImageCoord.y; y++)
        {
            for( int x = startImageCoord.x; x <= endImageCoord.x; x++)
            {
                outColor += (read_imagef(srcImg, sampler, (int2)(x, y)) * (kernelWeights[weight] / 71.844140f ));
                weight += 1;
            }
        }

        // Write the output value to image
        write_imagef(dstImg, outImageCoord, outColor);
    }
}
