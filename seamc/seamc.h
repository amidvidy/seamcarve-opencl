#ifndef _SEAMC_H_
#define _SEAMC_H_

#include "numcy.h"

#include <float.h>
#include <math.h>
#include <time.h>

/* Using a struct to avoid globals.  Might turn into something useful
 **   later when translating to various parallel/distributed versions.
 */
typedef struct SEAMC_WORK {
    time_t start_time, finish_time; // Epoch time
    clock_t start_clock, finish_clock; // CPU execution time (might break when using threads & GPU???)
            
    int height, width, xdim, ydim, xxdim, yydim;
    /* Could consider having matrices here too ?? */
} SEAMC_WORK_t, *SEAMC_WORK_p;

/* Core function headers for seam carving */

void SEAMC_mk_kernel(float** K);
void SEAMC_tfj_conv2d(SEAMC_WORK_p pWORK, float **I, float **O, float **K);
void SEAMC_dp(SEAMC_WORK_p pWORK, float **Y, float **G);
void SEAMC_copyKernel(SEAMC_WORK_p pWORK, float **I, int width_m1, int32_t *C);
void SEAMC_zeroKernel(float **Y, int h, int w);
void SEAMC_padKernel(float **OO, int h, int w);

float** SEAMC_carveGrey(float **iM, int iH, int iW, int newH, int newW);

#endif // _SEAMC_H_
