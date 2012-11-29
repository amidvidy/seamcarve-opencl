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
            
    int height, width, xdim, ydim;
    /* Could consider having matrices here too ?? */
} SEAMC_WORK_t, *SEAMC_WORK_p;

/* Core function headers for seam carving */

void SEAMC_dp(float **Y, float **G, short width, int height);
void SEAMC_backtrack(int *O, float **Y, short width, int height);
void SEAMC_carveKernel(void **DST, void **SRC, int width, int height, int32_t *CARVE, int pixBytes);
void SEAMC_lineKernel(void **DST, void **SRC, int width, int height, int32_t *CARVE, int pixBytes);
void SEAMC_zeroKernel(void **Y, short width, int height, int pixBytes);

void** SEAMC_carve(void **iM, int iW, int iH, int newW, int newH, bool isCOLOR = true,
        bool drawLINE = false);

#endif // _SEAMC_H_
