// Computes gradient of srcImage, writing result to resultMatrix

__kernel void image_gradient(__global uchar4* srcImg,
                             __global float* resultMatrix,
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
     float4 luma_coef = (float4)(0.299f, 0.587f, 0.114f, 0.0f);

#define getLum(pixel) ((uchar)0.299f * (pixel.x) + (uchar)0.587f * (pixel.y) + (uchar)0.114f * (pixel.z))

     // This is still broken.
     if (PixelCoord.x < width && PixelCoord.y < height) {
         // get luminescence values for pixels:
         uchar4 leftPixel = srcImg[leftPixelCoord.y * width + max(leftPixelCoord.x, 0)];
         float leftLum = (float)leftPixel.x * 0.299f + (float)leftPixel.y * 0.587f + (float)leftPixel.z * 0.114f;

         uchar4 rightPixel = srcImg[rightPixelCoord.y * width + min(rightPixelCoord.x, height - 1)];
         float rightLum = (float)rightPixel.x * 0.299f + (float)rightPixel.y * 0.587f + (float)rightPixel.z * 0.114f;

         uchar4 abovePixel =  srcImg[min(abovePixelCoord.y, height - 1) * width + abovePixelCoord.x];
         float aboveLum = (float)abovePixel.x * 0.299f + (float)abovePixel.y * 0.587f + (float)abovePixel.z * 0.114f;

         uchar4 belowPixel =  srcImg[max(belowPixelCoord.y, 0) * width + belowPixelCoord.x];
         float belowLum = (float)belowPixel.x * 0.299f + (float)belowPixel.y * 0.587f + (float)abovePixel.z * 0.114f;
         float gradient = fabs(rightLum - leftLum) + fabs(aboveLum - belowLum);

         resultMatrix[PixelCoord.x + width * PixelCoord.y] = gradient;
    }
}
