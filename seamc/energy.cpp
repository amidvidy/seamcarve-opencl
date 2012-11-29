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

void SEAMC_glaplauxian(const F4_t **srcImg, float** resultMatrix,
                              const int width, const int height)
{
    static const float extra = 1024.0f;
    static const F4_t luma_coef(0.299f * extra, 0.587f * extra, 0.114f * extra, 0.0f * extra);

    // Laplacian Gaussian Kernel is:


    static const 
    float kernelWeights[81] = {0.0f, 1.0f, 1.0f, 2.0f,   2.0f,   2.0f,   1.0f, 1.0f, 0.0f,
                               1.0f, 2.0f, 4.0f, 5.0f,   5.0f,   5.0f,   4.0f, 2.0f, 1.0f,
                               1.0f, 4.0f, 5.0f, 3.0f,   0.0f,   3.0f,   5.0f, 4.0f, 1.0f,
                               2.0f, 5.0f, 3.0f, -12.0f, -24.0f, -12.0f, 3.0f, 5.0f, 2.0f,
                               2.0f, 5.0f, 0.0f, -24.0f, -40.0f, -24.0f, 0.0f, 5.0f, 2.0f,
                               2.0f, 5.0f, 3.0f, -12.0f, -24.0f, -12.0f, 3.0f, 5.0f, 2.0f,
                               1.0f, 4.0f, 5.0f, 3.0f,   0.0f,   3.0f,   5.0f, 4.0f, 1.0f,
                               1.0f, 2.0f, 4.0f, 5.0f,   5.0f,   5.0f,   4.0f, 2.0f, 1.0f,
                               0.0f, 1.0f, 1.0f, 2.0f,   2.0f,   2.0f,   1.0f, 1.0f, 0.0f};
    
    for (int y = 0; y < height; y++) {
        float *pResultRow = resultMatrix[y];
        
        for (int x = 0; x < width; x++) {            
            float gauss_laplacian = 0.0f;
            int weight = 0;
            
            // Apply a -4..+4 x -4..+4 stencil thingy
            for (int yy = y-4; yy <= y+4; yy++) {
                for (int xx = x-4; xx <= x+4; xx++) {
                    const F4_t pix = readImage4Clip(srcImg, xx, yy, width, height);
                    const float lum = dot4(luma_coef, pix);
                    gauss_laplacian += lum * kernelWeights[weight++]; // Use & advance to next weight
                }
            }

            // Write the output value to the result Matrix:
            pResultRow[x] = gauss_laplacian + 14096.0f;
        }
    }
}

