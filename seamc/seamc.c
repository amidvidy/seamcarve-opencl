#include "seamc.h"

/* K is a 5x5 grid of floats (-2..-2 each axis in theory).
** Expected to be array of arrays at the moment.
**   Might change to a linear run of floats instead?
*/
void mk_kernel(float** K)
{
	float s, c0, c1, *pK;
	int x, y, xm, ym, dimX = 5, dimY = 5;
	
	s = 2.3;
	c0 = 1.0 / (2.0 * M_PI * s*s);
	c1 = -1.0 / (2.0 * s*s);
	for (y = 0; y < 5; y++) {
		ym = y - 2;
		for (x = 0; x < 5; x++) {
			xm = x - 2;
			pK = &K[y][x];
			//pK = K + (y*5) + x;
			*pK = c0 * exp((xm*xm + ym*ym) * c1);
		}
	}
} /* SOURCE:
def mk_kernel(K):
    s = 2.3;
    c0 = 1.0 / (2.0*math.pi*s*s);
    c1 = -1.0 / (2.0*s*s);
    for y in range(0,5):
        ym = y - 2;
        for x in range(0,5):
            xm = x - 2;
            K[y][x] = c0 * math.exp((xm*xm + ym*ym)*c1);
*/


void tfj_conv2d(float **I, float **O, float **K)
{

} /* SOURCE:
def tfj_conv2d(I,O,K):
    for y in range(3,ydim):
        for x in range(3,xdim):
            for yy in range(-2,3):
                for xx in range(-2, 3):
                    O[y][x] = O[y][x] + K[2+yy][2+xx] * I[y+yy][x+xx];
*/


void dp(float **Y, float **G)
{
} /* SOURCE:
def dp(Y,G):
    for i in range(5,yydim):
        for j in range(5,xxdim):
            Y[i][j] = G[i][j] + min(min(Y[i-1][j-1], Y[i-1][j]),Y[i-1][j+1]);
*/

void copyKernel(float **I, int width_m1, int c)
{
} /* SOURCE:
def copyKernel(I,width_m1,c):
    for i in range(0,height):
        for j in range(c, width_m1):
            I[i][j] = I[i][j+1]
*/

void zeroKernel(float **Y, int h, int w)
{
} /* SOURCE:
def zeroKernel(Y,h,w):
    for i in range(0,h):
        for j in range(0,w):
            Y[i][j] = 0
*/

void padKernel(float **OO, int h, int w)
{
} /* SOURCE:
def padKernel(OO,h,w):
    for i in range(0,h):
        for j in range(0,20):
            OO[i][j] = 1000000.0;
            OO[i][w-j-1] = 1000000.0;
*/

