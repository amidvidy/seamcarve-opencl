/* Original code by David Sheffield (UC Bereley):
 **
 ** Translated from Python to C...then onward.
 */

#include "energy_grey.h"
#include "seamc.h"
#include "numcy.h"

#include <stdio.h>
#include <string.h>

#define _PI_ 3.14159265

/* K is a 5x5 grid of floats (-2..-2 each axis in theory).
 ** Expected to be array of arrays at the moment.
 **   Might change to a linear run of floats instead?
 */
void SEAMC_mKONV_kernel(float** K)
{
    float s, c0, c1, *pK_y;
    int dimX = 5, dimY = 5;
    
    s = 2.3;
    c0 = 1.0 / (2.0 * _PI_ * s * s);
    c1 = -1.0 / (2.0 * s * s);
    for (int y = 0; y < dimY; y++) {
        int ym = y - 2;
        pK_y = K[y];
        for (int x = 0; x < dimX; x++) {
            int xm = x - 2;
            pK_y[x] = c0 * exp((xm * xm + ym * ym) * c1);
        }
    }
} // def mk_kernel(K):

/*
** from{Row,Col} is inclusive, to{Row,Col} is non-inclusive
*/
void SEAMC_tfj_conv2d(int fromRow, int fromCol, int toRow, int toCol, 
			float **I, float **O, float **K)
{
    float *pO_y, *pI_yyy, *pK_yy2;
    
    for (int y = fromRow; y < toRow; y++) {
        pO_y = O[y];
        for (int x = fromCol; x < toCol; x++) {
            for (int yy = -2; yy < 3; yy++) {
                pI_yyy = I[y + yy];
                pK_yy2 = K[yy + 2];
                for (int xx = -2; xx < 3; xx++) {
                    pO_y[x] += pK_yy2[xx + 2] * pI_yyy[x + xx];
                }
            }
        }
    }
} // def tfj_conv2d(I,O,K):

