/* Original code by David Sheffield (UC Bereley):
 **
 ** Translated from Python to C...then onward.
 */

#include "energy.h"
#include "seamc.h"
#include "numcy.h"

#include <stdio.h>
#include <string.h>

#define _PI_ 3.14159265

// Laplacian of Gaussian convolution of image:

void SEAMC_glaplauxian( //
        float** resultMatrix, const F4_t **srcImg, //
        const int width, const int height)
{
    static const float X = 1024.0f;
    static const F4_t luma_coef(//
            0.299f * X, 0.587f * X, 0.114f * X, 0.0f * X
            );
    
    // Laplacian Gaussian Kernel is:
    
    static const
    float kernelWeights[81] = { 0.0f, 1.0f, 1.0f, 2.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 2.0f, 4.0f, 5.0f, 5.0f, 5.0f, 4.0f, 2.0f, 1.0f,
        1.0f, 4.0f, 5.0f, 3.0f, 0.0f, 3.0f, 5.0f, 4.0f, 1.0f,
        2.0f, 5.0f, 3.0f, -12.0f, -24.0f, -12.0f, 3.0f, 5.0f, 2.0f,
        2.0f, 5.0f, 0.0f, -24.0f, -40.0f, -24.0f, 0.0f, 5.0f, 2.0f,
        2.0f, 5.0f, 3.0f, -12.0f, -24.0f, -12.0f, 3.0f, 5.0f, 2.0f,
        1.0f, 4.0f, 5.0f, 3.0f, 0.0f, 3.0f, 5.0f, 4.0f, 1.0f,
        1.0f, 2.0f, 4.0f, 5.0f, 5.0f, 5.0f, 4.0f, 2.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 2.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.0f };
    
    const IMG4_t SRC(srcImg, width, height);
    for (int y = 0; y < height; y++) {
        float *pResultRow = resultMatrix[y];
        for (int x = 0; x < width; x++) {
            float gauss_laplacian = 0.0f;
            int weight = 0;
            
            // Apply a -4..+4 x -4..+4 stencil thingy
            for (int yy = y - 4; yy <= y + 4; yy++) {
                for (int xx = x - 4; xx <= x + 4; xx++) {
                    const F4_t pix = readImage4Clip(SRC, xx, yy);
                    const float lum = dot4(luma_coef, pix);
                    gauss_laplacian += lum * kernelWeights[weight++]; // Use & advance to next weight
                }
            }
            
            // Write the output value to the result Matrix:
            pResultRow[x] = gauss_laplacian + 14096.0f;
        }
    }
}

void SEAMC_gaussian( //
        F4_t** resultImage, const F4_t **srcImg, //
        const int width, const int height)
{
    static const float X = 1.0f / 16.0f;
    static const
    float kernelWeights[9] = { //
            1.0f * X, 2.0f * X, 1.0f * X, //
            2.0f * X, 4.0f * X, 2.0f * X, //
            1.0f * X, 2.0f * X, 1.0f * X };
    
    const IMG4_t SRC(srcImg, width, height);
    for (int y = 0; y < height; y++) {
        F4_t *pResultRow = resultImage[y];
        for (int x = 0; x < width; x++) {
            
            int weight = 0;
            F4_t outColor(0.0f, 0.0f, 0.0f, 0.0f);
            
            // Apply a -1..+1 x -1..+1 stencil thingy
            for (int yy = y - 1; yy <= y + 1; yy++) {
                for (int xx = x - 1; xx <= x + 1; xx++) {
                    F4_t pix = readImage4Clip(SRC, xx, yy);
                     pix *= kernelWeights[weight++];
                     outColor += pix;
                }
            }
            // Write the output value to the result Matrix:
            pResultRow[x] = outColor;
        }
    }
}

void SEAMC_gradient( //
        float** resultMatrix, const F4_t **srcImg, //
        const int width, const int height)
{
    static const float X = 1.0f;
    static const F4_t luma_coef(//
            0.299f * X, 0.587f * X, 0.114f * X, 0.0f * X
            );

    const IMG4_t SRC(srcImg, width, height);
    for (int y = 0; y < height; y++) {
        float *pResultRow = resultMatrix[y];
        for (int x = 0; x < width; x++) {
             // Determine what portion of image to operate on:
             I2_t leftPixelCoord (x - 1, y);
             I2_t rightPixelCoord(x + 1, y);
             I2_t abovePixelCoord(x, y + 1);
             I2_t belowPixelCoord(x, y - 1);
             I2_t PixelCoord(x, y);     // This is location of pixel whose gradient is being computed.

                 // get luminescence values for pixels:
                 float leftpixel = dot4(luma_coef, readImage4Clip(SRC, leftPixelCoord));
                 float rightpixel = dot4(luma_coef,readImage4Clip(SRC, rightPixelCoord));
                 float abovepixel = dot4(luma_coef, readImage4Clip(SRC,  abovePixelCoord));
                 float belowpixel = dot4(luma_coef, readImage4Clip(SRC, belowPixelCoord));
                 float gradient = fabs(rightpixel - leftpixel) + fabs(abovepixel - belowpixel);
                 gradient *= 1024.0f;
                 gradient += 256;

                 pResultRow[x] = gradient;
        }
    }
}
