#ifndef _ENERGY_H_
#define _ENERGY_H_

#include "numcy.h"

#include <float.h>
#include <math.h>
#include <time.h>


void SEAMC_mk_kernel(float** K);
void SEAMC_tfj_conv2d(int fromRow, int fromCol, int toRow, int toCol, 
                        float **I, float **O, float **K);

#endif // _ENERGY_H__

