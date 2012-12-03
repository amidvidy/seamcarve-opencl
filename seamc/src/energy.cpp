/* Original code by David Sheffield (UC Bereley):
 **
 ** Translated from Python to C...then onward.
 */

#include "energy.h"
#include "seamc.h"
#include "numcy.h"

#include <stdio.h>
#include <string.h>

#include <omp.h>

#define _PI_ 3.14159265

// Randomly chosen (perhaps rather small) block sizes for cache-block
static const int block_x = 64, block_y = 12;

// Laplacian of Gaussian convolution of image:

void SEAMC_glaplauxian( //
        float** resultMatrix, const F4_t **srcImg, //
        const int width, const int height)
{
    static const float X = 1024.0f;
    static const F4_t luma_coef( //
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
    
    // BEGIN: CACHE BLOCKING GRID
#pragma omp parallel for
    for (int big_y = 0; big_y < height; big_y += block_y) {
        int end_y = big_y + block_y;
        if (end_y > height) end_y = height;
        
        for (int big_x = 0; big_x < width; big_x += block_x) {
            int end_x = big_x + block_x;
            if (end_x > width) end_x = width;
            // END: CACHE BLOCKING GRID
            
            for (int y = big_y; y < end_y; y++) {
                float *pResultRow = resultMatrix[y];
                
                for (int x = big_x; x < end_x; x++) {

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
    
    // BEGIN: CACHE BLOCKING GRID
#pragma omp parallel for
    for (int big_y = 0; big_y < height; big_y += block_y) {
        int end_y = big_y + block_y;
        if (end_y > height) end_y = height;
        
        for (int big_x = 0; big_x < width; big_x += block_x) {
            int end_x = big_x + block_x;
            if (end_x > width) end_x = width;
            // END: CACHE BLOCKING GRID
            
            for (int y = big_y; y < end_y; y++) {
                F4_t *pResultRow = resultImage[y];
                
                for (int x = big_x; x < end_x; x++) {

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
    }
}

void SEAMC_gradient( //
        float** resultMatrix, const F4_t **srcImg, //
        const int width, const int height)
{
    static const F4_t luma_coef( //
            0.299f, 0.587f, 0.114f, 0.0f);
    
    const IMG4_t SRC(srcImg, width, height);
    
    // BEGIN: CACHE BLOCKING GRID
#pragma omp parallel for
    for (int big_y = 0; big_y < height; big_y += block_y) {
        int end_y = big_y + block_y;
        if (end_y > height) end_y = height;
        
        for (int big_x = 0; big_x < width; big_x += block_x) {
            int end_x = big_x + block_x;
            if (end_x > width) end_x = width;
            // END: CACHE BLOCKING GRID
            
            for (int y = big_y; y < end_y; y++) {
                float *pResultRow = resultMatrix[y];
                
                for (int x = big_x; x < end_x; x++) {

                    // Determine what portion of image to operate on:
                    I2_t leftPixelCoord(x - 1, y);
                    I2_t rightPixelCoord(x + 1, y);
                    I2_t abovePixelCoord(x, y + 1);
                    I2_t belowPixelCoord(x, y - 1);
                            
                    // get luminescence values for pixels:
                    float leftpixel = dot4(luma_coef, readImage4Clip(SRC, leftPixelCoord));
                    float rightpixel = dot4(luma_coef, readImage4Clip(SRC, rightPixelCoord));
                    float abovepixel = dot4(luma_coef, readImage4Clip(SRC, abovePixelCoord));
                    float belowpixel = dot4(luma_coef, readImage4Clip(SRC, belowPixelCoord));
                    //float gradient = fabs(rightpixel - leftpixel) + fabs(abovepixel - belowpixel);
                    // Slightly different formulation of gradient
                    float gradient = sqrt(
                            pow(rightpixel - leftpixel, 2) + pow(abovepixel - belowpixel, 2));
                    gradient *= 1024.0f;
                    gradient += 256;
                    
                    pResultRow[x] = gradient;
                }
            }
        }
    }
}

/*
void SEAMC_gaussgrad( //
        float** resultMatrix, const F4_t **srcImg, //
        const int width, const int height)
{
    static const float X = 1.0f / 16.0f;
    static const
    float kernelWeights[9] = { //
            1.0f * X, 2.0f * X, 1.0f * X, //
            2.0f * X, 4.0f * X, 2.0f * X, //
            1.0f * X, 2.0f * X, 1.0f * X };

    const IMG4_t SRC(srcImg, width, height);

    // BEGIN: CACHE BLOCKING GRID
    for (int big_y = 0; big_y < height; big_y += block_y) {
        int end_y = big_y + block_y;
        if (end_y > height) end_y = height;

        for (int big_x = 0; big_x < width; big_x += block_x) {
            int end_x = big_x + block_x;
            if (end_x > width) end_x = width;
            // END: CACHE BLOCKING GRID

            for (int y = big_y; y < end_y; y++) {
                F4_t *pResultRow = resultImage[y];

                for (int x = big_x; x < end_x; x++) {

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

            for (int y = big_y; y < end_y; y++) {
                float *pResultRow = resultMatrix[y];

                for (int x = big_x; x < end_x; x++) {

                    // Determine what portion of image to operate on:
                    I2_t leftPixelCoord(x - 1, y);
                    I2_t rightPixelCoord(x + 1, y);
                    I2_t abovePixelCoord(x, y + 1);
                    I2_t belowPixelCoord(x, y - 1);

                    // get luminescence values for pixels:
                    float leftpixel = dot4(luma_coef, readImage4Clip(SRC, leftPixelCoord));
                    float rightpixel = dot4(luma_coef, readImage4Clip(SRC, rightPixelCoord));
                    float abovepixel = dot4(luma_coef, readImage4Clip(SRC, abovePixelCoord));
                    float belowpixel = dot4(luma_coef, readImage4Clip(SRC, belowPixelCoord));
                    //float gradient = fabs(rightpixel - leftpixel) + fabs(abovepixel - belowpixel);
                    // Slightly different formulation of gradient
                    float gradient = sqrt(
                            pow(rightpixel - leftpixel, 2) + pow(abovepixel - belowpixel, 2));
                    gradient *= 1024.0f;
                    gradient += 256;

                    pResultRow[x] = gradient;
                }
            }

        }
    }
}
*/
