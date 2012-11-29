
// Laplacian of Gaussian convolution of image:

__kernel void gaussian_laplacian(__read_only image2d_t srcImg,
                              __global float* resultMatrix,
                              sampler_t sampler,
                              int width, int height)
{
     // TODO: the coefficient weights to convert to grayscale:
     // (should be ported to CPU?)
     float4 luma_coef = (float4) (0.299f, 0.587f, 0.114f, 0.0f);

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
    
    int2 tid = (int2) (get_global_id(0), get_global_id(1));		 
    int2 startImageCoord = tid - 4;
    int2 endImageCoord   = tid + 4;
    if (tid.x < width && tid.y < height)
    {
        float gauss_laplacian = 0.0f;
        int weight = 0;
        for( int y = startImageCoord.y; y <= endImageCoord.y; y++)
        {
            for( int x = startImageCoord.x; x <= endImageCoord.x; x++)
            {
	      gauss_laplacian += (dot(luma_coef, read_imagef(srcImg, sampler, (int2)(x, y))) * (kernelWeights[weight]));
                weight += 1;
            }
        }

        // Write the output value to the result Matrix:
        resultMatrix[tid.x + width*tid.y] = gauss_laplacian;
    }
}
