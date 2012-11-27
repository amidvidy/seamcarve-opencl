// Computes gradient of srcImage, writing result to resultMatrix

__kernel void image_gradient(__read_only image2d_t srcImg,
                             __global float* resultMatrix,
                             sampler_t sampler,
                             int width,
                             int height)
{


     // Determine what portion of image to operate on:
     int2 leftPixelCoord = (int2) (get_global_id(0) - 1, get_global_id(1));
     int2 rightPixelCoord  = (int2) (get_global_id(0) + 1, get_global_id(1));
     int2 abovePixelCoord = (int2) (get_global_id(0), get_global_id(1) + 1);
     int2 belowPixelCoord = (int2) (get_global_id(0), get_global_id(1) - 1);
     int2 PixelCoord = (int2) (get_global_id(0), get_global_id(1));     // This is location of pixel whose gradient is being computed.
     //     printf("START: global_id = (%i, %i)\n", get_global_id(0), get_global_id(1));
     // TODO: the coefficient weights to convert to grayscale:
     // (should be ported to CPU?)
     float4 luma_coef = (float4) (0.299f, 0.587f, 0.114f, 0.0f);

     if (PixelCoord.x < width && PixelCoord.y < height) {
         // get luminescence values for pixels:
         float leftpixel = dot(luma_coef, read_imagef(srcImg, sampler, leftPixelCoord));
         float rightpixel = dot(luma_coef,read_imagef(srcImg, sampler, rightPixelCoord));
         float abovepixel = dot(luma_coef, read_imagef(srcImg, sampler, abovePixelCoord));
         float belowpixel = dot(luma_coef, read_imagef(srcImg, sampler, belowPixelCoord));
         // Slightly different formulation of gradient
         float gradient = sqrt(pow(rightpixel - leftpixel, 2) + pow(abovepixel - belowpixel, 2))

         resultMatrix[PixelCoord.x + width * PixelCoord.y] = gradient;
    }
}

