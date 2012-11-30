
// Gaussian filter of image

__kernel void gaussian_filter(__global uchar4* srcImg,
                              __global uchar4* dstImg,
                              int width,
                              int height,
                              int colsRemoved)
{
    // Gaussian Kernel is:
    // 1  2  1
    // 2  4  2
    // 1  2  1
    float kernelWeights[9] = { 1.0f, 2.0f, 1.0f,
                               2.0f, 4.0f, 2.0f,
                               1.0f, 2.0f, 1.0f };

    int2 startImageCoord = (int2) (get_global_id(0) - 1, get_global_id(1) - 1);
    int2 endImageCoord   = (int2) (get_global_id(0) + 1, get_global_id(1) + 1);
    int2 outImageCoord = (int2) (get_global_id(0), get_global_id(1));

    if (outImageCoord.x < (width - colsRemoved) && outImageCoord.y < height)
    {
        int weight = 0;
        float4 outColor = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
        float r = 0, g = 0, b = 0;
        for( int y = startImageCoord.y; y <= endImageCoord.y; y++)
        {
            for( int x = startImageCoord.x; x <= endImageCoord.x; x++)
            {
                filterElem = kernelWeights[weight];
                uchar4 sourcePixel = srcImg[clamp(y, 0, height) * width + clamp(x, 0, width)];
                r += sourcePixel.x * (filterElem / 16.0f);
                g += sourcePixel.y * (filterElem / 16.0f);
                b += sourcePixel.z * (filterElem / 16.0f);
                weight += 1;
            }
        }

        // Write the output value to image
        outImg[outImgCoord.y * width + outImageCoord.x] = (uchar4)(r, g, b, 0);
    }
}
