#include <float.h>
#include <math.h>

void mk_kernel(float** K);
void tfj_conv2d(float **I, float **O, float **K);
void dp(float **Y, float **G);
void copyKernel(float **I, int width_m1, int c);
void zeroKernel(float **Y, int h, int w);
void padKernel(float **OO, int h, int w);

