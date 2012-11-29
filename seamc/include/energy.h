#ifndef _ENERGY_H_
#define _ENERGY_H_

#include "numcy.h"

#include "seamc.h"

#include <float.h>
#include <math.h>
#include <time.h>

void SEAMC_glaplauxian( //
        float** resultMatrix, const F4_t **srcImg, //
        const int width, const int height);
void SEAMC_gaussian( //
        F4_t** resultImage, const F4_t **srcImg, //
        const int width, const int height);
void SEAMC_gradient( //
        float** resultMatrix, const F4_t **srcImg, //
        const int width, const int height);

#endif // _ENERGY_H_

