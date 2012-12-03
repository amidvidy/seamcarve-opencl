
// Sobel Operator convolution of image = gradient of
// Gaussian convolution:

__kernel void gaussian_filter(__global uchar4* srcImg,
                              __global float* resultMatrix,
                              int width,
                              int height,
                              int colsRemoved)
{
    float SobelXOperator[9] = {-1.0f, 0.0f, 1.0f,
                               -2.0f, 0.0f, 2.0f,
                               -1.0f, 0.0f, 1.0f };

    float SobelYOperator[9] = {-1.0f, -2.0f, -1.0f,
                               0.0f, 0.0f, 0.0f,
                               1.0f, 2.0f, 1.0f };

    float4 luma_coef = (float4)(0.299f, 0.587f, 0.114f, 0.0f);

#define getLum(pixel) ((uchar)0.299f * (pixel.x) + (uchar)0.587f * (pixel.y) + (uchar)0.114f * (pixel.z))

    int2 startImageCoord = (int2) (get_global_id(0) - 1, get_global_id(1) - 1);
    int2 endImageCoord   = (int2) (get_global_id(0) + 1, get_global_id(1) + 1);
    int2 outImageCoord = (int2) (get_global_id(0), get_global_id(1));

    if (outImageCoord.x < (width - colsRemoved) && outImageCoord.y < height)
    {
        int weight = 0;
	x_convolution = 0;
	y_convolution = 0;
        for( int y = startImageCoord.y; y <= endImageCoord.y; y++)
        {
            for( int x = startImageCoord.x; x <= endImageCoord.x; x++)
            {
		float x_weight = SobelXOperator[weight];
		float y_weight = SobelYOperator[weight];

                uchar4 sourcePixel = srcImg[clamp(y, 0, height-1) * width + clamp(x, 0, width-1)];
		x_convolution += 
	               getLum(sourcePixel)*x_weight;
		y_convolution += getLum(sourcePixel)*y_weight;		      weight += 1;
	    }
	} 
        // Write the gradient value to matrix
        resultMatrix[outImageCoord.x + width * outImageCoord.y] = fabs(x_convolution) + fabs(y_convolution);
    }
}
