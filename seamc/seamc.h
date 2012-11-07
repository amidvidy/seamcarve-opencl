#include <float.h>
#include <math.h>
#include <time.h>


/* Using a struct to avoid globals.  Might turn into something useful
**   later when translating to various parallel/distributed versions.
*/
typedef struct SEAMC_WORK {
	time_t		start_time, finish_time;
	int		height, width, xdim, ydim, xxdim, yydim;
	/* Could consider having matrices here too ?? */
} SEAMC_WORK_t, *SEAMC_WORK_p;


/* Core functions for seam carving */

void mk_kernel(float** K);
void tfj_conv2d(float **I, float **O, float **K);
void dp(float **Y, float **G);
void copyKernel(float **I, int width_m1, int c);
void zeroKernel(float **Y, int h, int w);
void padKernel(float **OO, int h, int w);

