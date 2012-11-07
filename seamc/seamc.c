#include "seamc.h"

#define pi 3.14

void mk_kernel(float*[5] K) {
	float s, c0, c1;
	int x, y, xm, ym;
	
	s = 2.3;
	c0 = 1.0 / (2.0*pi*s*s);
	c1 = -1.0 / (2.0*s*s);
	for (y = 0; y < 5; y++) {
		ym = y - 2;
		for (x = 0; x < 5; x++) {
			xm = x - 2;
			K[y][x] = c0 * exp((xm*xm + ym*ym)*c1);
		}
	}
	
}

