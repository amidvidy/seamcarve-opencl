// Computes gradient of srcImage, writing result to resultMatrix

__kernel void image_gradient(__global uchar4* srcImg,
                             __global float* resultMatrix,
                             sampler_t sampler,
                             int width,
                             int height,
                             int colsRemoved)
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
     uchar4 luma_coef = ((uchar)0.299f, (uchar)0.587f, (uchar)0.114f, (uchar)0.0f);


     // This is still broken.
     if (PixelCoord.x < width && PixelCoord.y < height) {
         // get luminescence values for pixels:
         uchar leftPixel = dot(luma_coef, srcImg[
         uchar leftpixel = dot(luma_coef, read_imagef(srcImg, sampler, leftPixelCoord));
         float rightpixel = dot(luma_coef,read_imagef(srcImg, sampler, rightPixelCoord));
         float abovepixel = dot(luma_coef, read_imagef(srcImg, sampler, abovePixelCoord));
         float belowpixel = dot(luma_coef, read_imagef(srcImg, sampler, belowPixelCoord));
         float gradient = fabs(rightpixel - leftpixel) + fabs(abovepixel - belowpixel);

         resultMatrix[PixelCoord.x + width * PixelCoord.y] = gradient;
    }
}
