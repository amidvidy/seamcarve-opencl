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
     int2 abovePixelCoord = (int2) (get_global_id(0), get_gloabal_id(1) + 1);
     int2 belowPixelCoord = (int2) (get_gloabl_id(0), get_global_id(1) - 1);
     int2 PixelCoord = (int2) (get_global_id(0), get_global_id(1));     // This is location of pixel whose gradient is being computed.

     // TODO: the coefficient weights to convert to grayscale:
     // (should be ported to CPU?)
     float4 luma_coef = (float4) (0.299f, 0.587f, 0.114f, 0.0f);

     if (PixelCoord.x < width && PixelCoord.y < height) {
         // get luminescence values for pixels:
         float leftpixel = dot(luma_coef, read_image(srcImg, sampler, leftPixelCoord));
         float rightpixel = dot(luma_coef,read_image(srcImg, sampler, rightPixelCoord));
         float abovepixel = dot(luma_coef, read_image(srcImg, sampler, abovePixelCoord));
         float belowpixel = dot(luma_coef, read_image(srcImg, sampler, belowPixelCoord));
         float gradient = abs(rightpixel - leftpixel) + abs(abovepixel - belowpixel);

         // write results to matrix:
         resultMatrix[get_global_id(0)][get_global_id(1)] = gradient;
     }
}
