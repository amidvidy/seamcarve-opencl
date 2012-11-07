#include "seamc.h"

#define pi 3.14

/* K is a 5x5 grid of floats (-2..-2 each axis in theory).
** Expected to be array of arrays at the moment.
**   Might change to a linear run of floats instead?
*/
void mk_kernel(float** K) {
	float s, c0, c1, *pK;
	int x, y, xm, ym, dimX = 5, dimY = 5;
	
	s = 2.3;
	c0 = 1.0 / (2.0*pi*s*s);
	c1 = -1.0 / (2.0*s*s);
	for (y = 0; y < 5; y++) {
		ym = y - 2;
		for (x = 0; x < 5; x++) {
			xm = x - 2;
			pK = &K[y][x];
			//pK = K + (y*5) + x;
			*pK = c0 * exp((xm*xm + ym*ym)*c1);
		}
	}
	
}

