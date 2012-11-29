#ifndef _ENERGY_H_
#define _ENERGY_H_

#include "numcy.h"

#include "seamc.h"

#include <float.h>
#include <math.h>
#include <time.h>

void SEAMC_glaplauxian(const F4_t **srcImg, float** resultMatrix,
              const int width, const int height);
        
#endif // _ENERGY_H_

