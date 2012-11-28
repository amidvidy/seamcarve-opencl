/* Original code by David Sheffield (UC Bereley):
 **
 ** Translated from Python to C...then onward.
 */

#include "seamc.h"
#include "energy_old.h"
#include "numcy.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>


void SEAMC_dp(SEAMC_WORK_p pWORK, float **Y, float **G)
{
    int height_m1 = pWORK->height-1, width_m1 = pWORK->width-1;
    float *pY_y, *pY_yp, *pG_y;
    
    pG_y = G[0];
    pY_y = Y[0];
    for (int x = 0; x <= width_m1; x++) {
        pY_y[x] = pG_y[x]; // Top row just gets copied
    }
    for (int y = 1; y <= height_m1; y++) {
        pG_y = G[y];
        pY_y = Y[y];
        pY_yp = Y[y - 1];
        
        pY_y[0] = fmin(pY_yp[0], pY_yp[1]);
        for (int x = 1; x < width_m1; x++) {
            float pathCost = fmin(fmin(pY_yp[x - 1], pY_yp[x]), pY_yp[x + 1]);
            pY_y[x] = pG_y[x] + pathCost;
        }
        pY_y[width_m1] = fmin(pY_yp[width_m1], pY_yp[width_m1-1]);
    }
} // def dp(Y,G):


void SEAMC_copyKernel(SEAMC_WORK_p pWORK, float **I, int width_m1, int32_t *C)
{
    int height = pWORK->height;
    float *pI_y;
    
    for (int y = 0; y < height; y++) {
        pI_y = I[y];
        for (int x = C[y]; x < width_m1; x++) {
            pI_y[x] = pI_y[x + 1];
        }
    }
} // def copyKernel(I,width_m1,c):


void SEAMC_zeroKernel(float **Y, int h, int w)
{
    float *pY_y;
    
    for (int y = 0; y < h; y++) {
        pY_y = Y[y];
        for (int x = 0; x < w; x++) {
            pY_y[x] = 0;
        }
    }
} // def zeroKernel(Y,h,w):


void SEAMC_padKernel(float **OO, int h, int w)
{
    int ylast = w - 1;
    float *pOO_y;
    
    for (int y = 0; y < h; y++) {
        pOO_y = OO[y];
        for (int x = 0; x < 20; x++) {
            pOO_y[x] = 1000000.0;
            pOO_y[ylast - x] = 1000000.0;
        }
    }
} // def padKernel(OO,h,w):


void SEAMC_backtrack(SEAMC_WORK_p pWORK, float **Y, int *O)
{
    int width_m1 = pWORK->width-1, height_m1 = pWORK->height-1;
    float min_v, L, C, R, *pY;
    int idx = 0;
    
    int y = height_m1;
    int x = idx;
    pY = Y[y];
    min_v = pY[x];
    while (++x <= width_m1) {
        if (pY[x] < min_v) {
            min_v = pY[x];
            idx = x;
        }
    }
    
    /* printf("idx=%d, min_v=%f\n", idx, min_v); */
    O[y] = idx;
    while (--y >= 0) {
        pY = Y[y];
        L = (idx < 1) ? FLT_MAX : pY[idx - 1];
        C = pY[idx];
        R = (idx >= width_m1) ? FLT_MAX : pY[idx + 1];
        
        if (L < C) {
            idx += (L < R) ? -1 : 1;
        } else {
            idx += (C < R) ? 0 : 1;
        }
        /* printf("i=%d,idx=%d\n", i, idx); */
        O[y] = idx;
    }
} // def backtrack(Y,O):


/*
** WARNING: Modifies input matrix too :)
*/
float** SEAMC_carveGrey(float **iM, int iH, int iW, int newH, int newW)
{
//TODO: Error handling (out of memory, etc)
//TODO: perhaps use the output matrix as the working copy rather than modifying the input matrix.

    float** newM = np_zero_matrix_float(newH, newW, NULL );
    
    if (0) {    // Quick test of stuff!
        for (int y = 0; y < newH; y++) {
            float *iROW = iM[iH - 1 - y], *oROW = newM[y];
            for (int x = 0; x < newW; x++) {
                oROW[x] = iROW[iW - 1 - x];
            }
        }
        return newM;
    }
    
    float** K = np_zero_matrix_float(5, 5, NULL );
    SEAMC_mk_kernel(K); // Could even be done once statically
            
    SEAMC_WORK_t WORK; // Consistent values across multiple SEAMC calls (rather than globals)
    
    int32_t* B = np_zero_array_int32(iH);
    
    int num_carveH = iW - newW, num_carveV = iH - newH;
    int disableTFJ = 0; // Not referenced elsewhere?
    
    float** O = np_zero_matrix_float(iH, iW, NULL );
    float** OO = np_zero_matrix_float(iH, iW, NULL );
    
    WORK.width = iW;
    WORK.height = iH;
    while (WORK.width > newW) {   // TODO: Deal with stretch & vertical too!!!
        WORK.start_time = time(NULL ); // Epoch time
        WORK.start_clock = clock(); // CPU usage
        WORK.ydim = WORK.height - 3;
        WORK.xdim = WORK.width - 3;
        
        SEAMC_zeroKernel(O, newH, newW);
        SEAMC_zeroKernel(OO, newH, newW);
        
        SEAMC_tfj_conv2d(3, 3, WORK.ydim, WORK.xdim, iM, O, K);
        
        //SEAMC_padKernel(OO, WORK.height, WORK.width);
        for (int y = 0; y < WORK.height; y++) {
            for (int x = 0; x < 20; x++) {
                OO[y][x] = 1000000.0;
                OO[y][WORK.width - 1 - x] = 1000000.0;
            }
        }
        
        SEAMC_dp(&WORK, OO, O);
        
        time_t t0 = time(NULL );
        SEAMC_backtrack(&WORK, OO, B);
        double secs = difftime(time(NULL ), t0);
        printf("%f sec in backtrack (c function)\n", secs);
        
        SEAMC_copyKernel(&WORK, iM, WORK.width - 1, B); // Was B[i] utilizing "i" from copyKernel's loop!
                
        WORK.width -= 1;
        double elapsed = difftime(time(NULL ), WORK.start_time);
        printf("%f sec this iteration\n", elapsed);
    }
    
    for (int y = 0; y < WORK.height; y++) {
        memmove(newM[y], iM[y], newW * sizeof(float));
    }
    return newM;
}

