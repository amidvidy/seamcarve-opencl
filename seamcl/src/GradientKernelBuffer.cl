// Computes Sobel convolution of srcImage, writing result to resultMatrix:

__kernel void image_gradient(__global uchar4* srcImg,
                             __global float* resultMatrix,
                             int width,
                             int height,
                             int colsRemoved)
{
     // Determine what portion of image to operate on:
     int x = get_global_id(0);
     int y = get_global_id(1);
     int x_left = max(x-1,0);
     int x_right = min(x+1,height-1);
     int y_below = max(y-1,0);
     int y_above = min(y+1,height-1);

       if (x < width && y < height) {
         // get luminescence values for pixels:
       uchar4 belowLeftPixel = srcImg[y_below * width + x_left];
         uchar4 belowPixel = srcImg[y_below * width + x];
         uchar4 belowRightPixel = srcImg[y_below * width + x_right];
         uchar4 leftPixel = srcImg[y * width + x_left];
         uchar4 rightPixel = srcImg[y * width + x_right];
         uchar4 aboveLeftPixel = srcImg[y_above * width + x_left];
         uchar4 abovePixel =  srcImg[y_above * width + x];
         uchar4 aboveRightPixel = srcImg[y_above * width + x_right];

   // get luminance values:
         float belowLeftLum = (float)belowLeftPixel.x * 0.299f + (float)belowLeftPixel.y * 0.587f + (float)belowLeftPixel.z * 0.114f;
         float belowLum = (float)belowPixel.x * 0.299f + (float)belowPixel.y * 0.587f + (float)abovePixel.z * 0.114f;
         float belowRightLum = (float)belowRightPixel.x * 0.299f + (float)belowRightPixel.y * 0.587f + (float)belowRightPixel.z * 0.114f;
         float leftLum = (float)leftPixel.x * 0.299f + (float)leftPixel.y * 0.587f + (float)leftPixel.z * 0.114f;
         float rightLum = (float)rightPixel.x * 0.299f + (float)rightPixel.y * 0.587f + (float)rightPixel.z * 0.114f;
         float aboveLeftLum = (float)aboveLeftPixel.x * 0.299f + (float)aboveLeftPixel.y * 0.587f + (float)aboveLeftPixel.z * 0.114f;
         float aboveLum = (float)abovePixel.x * 0.299f + (float)abovePixel.y * 0.587f + (float)abovePixel.z * 0.114f;
         float aboveRightLum = (float)aboveRightPixel.x * 0.299f + (float)aboveRightPixel.y * 0.587f + (float)aboveRightPixel.z * 0.114f;
         //float gradient = fabs(rightLum - leftLum) + fabs(aboveLum - belowLum);

float sobel_gradient = fabs(belowRightLum - belowLeftLum + aboveRightLum - aboveLeftLum + 2*(rightLum - leftLum)) + fabs(aboveLeftLum - belowLeftLum + aboveRightLum - belowRightLum + 2*(aboveLum - belowLum));

         resultMatrix[x + width * y] = sobel_gradient;
    }
}
