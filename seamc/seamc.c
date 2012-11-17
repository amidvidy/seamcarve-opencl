/* Original code by David Sheffield (UC Bereley):
 **
 ** Translated from Python to C...then onward.
 */

#include "seamc.h"
#include "numcy.h"

#include <stdio.h>

#define _PI_ 3.14159265

/* K is a 5x5 grid of floats (-2..-2 each axis in theory).
 ** Expected to be array of arrays at the moment.
 **   Might change to a linear run of floats instead?
 */
void SEAMC_mk_kernel(float** K)
{
    float s, c0, c1, *pK_y;
    int x, y, xm, ym, dimX = 5, dimY = 5;
    
    s = 2.3;
    c0 = 1.0 / (2.0 * _PI_ * s * s);
    c1 = -1.0 / (2.0 * s * s);
    for (y = 0; y < dimY; y++) {
        ym = y - 2;
        pK_y = K[y];
        for (x = 0; x < dimX; x++) {
            xm = x - 2;
            pK_y[x] = c0 * exp((xm * xm + ym * ym) * c1);
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

void SEAMC_tfj_conv2d(SEAMC_WORK_p pWORK, float **I, float **O, float **K)
{
    float *pO_y, *pI_yyy, *pK_yy2;
    int y, x, yy, xx, ydim = pWORK->ydim, xdim = pWORK->xdim;
    
    for (y = 3; y < ydim; y++) {
        pO_y = O[y];
        for (x = 3; y < xdim; x++) {
            for (yy = -2; yy < 3; yy++) {
                pI_yyy = I[y + yy];
                pK_yy2 = K[yy + 2];
                for (xx = -2; xx < 3; xx++) {
                    pO_y[x] += pK_yy2[xx + 2] * pI_yyy[x + xx];
                }
            }
        }
    }
} /* SOURCE:
 def tfj_conv2d(I,O,K):
 for y in range(3,ydim):
 for x in range(3,xdim):
 for yy in range(-2,3):
 for xx in range(-2, 3):
 O[y][x] = O[y][x] + K[2+yy][2+xx] * I[y+yy][x+xx];
 */

void SEAMC_dp(SEAMC_WORK_p pWORK, float **Y, float **G)
{
    float *pY_i, *pY_ip, *pG_i;
    int i, j, yydim = pWORK->yydim, xxdim = pWORK->xxdim;
    
    for (i = 5; i < yydim; i++) {
        pY_i = Y[i];
        pY_ip = Y[i - 1];
        pG_i = G[i];
        for (j = 5; j < xxdim; j++) {
            pY_i[j] = pG_i[j]
                    + fmin(fmin(pY_ip[j - 1], pY_ip[j]), pY_ip[j + 1]);
        }
    }
} /* SOURCE:
 def dp(Y,G):
 for i in range(5,yydim):
 for j in range(5,xxdim):
 Y[i][j] = G[i][j] + min(min(Y[i-1][j-1], Y[i-1][j]),Y[i-1][j+1]);
 */

void SEAMC_copyKernel(SEAMC_WORK_p pWORK, float **I, int width_m1, int c)
{
    float *pI_i;
    int i, j, height = pWORK->height;
    
    for (i = 0; i < height; i++) {
        pI_i = I[i];
        for (j = c; j < width_m1; j++) {
            pI_i[j] = pI_i[j + 1];
        }
    }
} /* SOURCE:
 def copyKernel(I,width_m1,c):
 for i in range(0,height):
 for j in range(c, width_m1):
 I[i][j] = I[i][j+1]
 */

void SEAMC_zeroKernel(float **Y, int h, int w)
{
    float *pY_i;
    int i, j;
    
    for (i = 0; i < h; i++) {
        pY_i = Y[i];
        for (j = 0; j < w; j++) {
            pY_i[j] = 0;
        }
    }
} /* SOURCE:
 def zeroKernel(Y,h,w):
 for i in range(0,h):
 for j in range(0,w):
 Y[i][j] = 0
 */

void SEAMC_padKernel(float **OO, int h, int w)
{
    float *pOO_i;
    int i, j;
    
    for (i = 0; i < h; i++) {
        pOO_i = OO[i];
        for (j = 0; j < 20; j++) {
            pOO_i[j] = 1000000.0;
            pOO_i[w - j - 1] = 1000000.0;
        }
    }
} /* SOURCE:
 def padKernel(OO,h,w):
 for i in range(0,h):
 for j in range(0,20):
 OO[i][j] = 1000000.0;
 OO[i][w-j-1] = 1000000.0;
 */

void SEAMC_backtrack(SEAMC_WORK_p pWORK, float **Y, int *O)
{
    float min_v, L, C, R, *pY;
    int idx, i, y, width = pWORK->width, yydim = pWORK->yydim;
    
    idx = 5;
    min_v = 100000000.0;
    
    pY = Y[yydim - 1];
    for (i = idx; i < (width - 5); i++) {
        if (pY[i] < min_v) {
            min_v = pY[i];
            idx = i;
        }
    }
    
    /* printf("idx=%d, min_v=%f", idx, min_v); */
    O[yydim - 1] = idx;
    
    for (y = 2; y < (yydim - 1); y++) {
        i = (yydim - y);
        
        pY = Y[i];
        L = pY[idx - 1];
        C = pY[idx];
        R = pY[idx + 1];
        
        if (L < C) {
            idx += (L < R) ? -1 : 1;
        } else {
            idx += (C < R) ? 0 : 1;
        }
        
        /* printf("i=%d,idx=%d", i, idx); */
        if (idx > (width - 5)) idx = (width - 5);
        if (idx < 5) idx = 5;
        O[i] = idx;
    }
} /* SOURCE:
 def backtrack(Y,O):
 idx = 5;
 min_v = 100000000.0;
 
 for i in range(idx,(width-5)):
 if(Y[yydim-1][i] < min_v):
 min_v = Y[yydim-1][i]
 idx = i
 
 #print 'idx=%d, min_v=%f' % (idx, min_v)
 O[yydim-1] = idx;
 

 for y in range(2, (yydim-1)):
 i = (yydim - y);
 
 L = Y[i][idx-1];
 C = Y[i][idx];
 R = Y[i][idx+1];

 if(L < C):
 if(L < R):
 idx = idx-1;
 else:
 idx = idx+1;
 else:
 if(C < R):
 idx = idx;
 else:
 idx = idx + 1;

 #print 'i=%d,idx=%d' % (i,idx)
 idx = min(idx, (width-5));
 idx = max(idx, 5);
 O[i] = idx;
 */

void SEAMC_carveGrey(float **iM, int iH, int iW, float **oM, int oH, int oW)
{
    if (0) {    // Quick test of stuff!
        for (int y = 0; y < oH; y++) {
            float *iROW = iM[iH - 1 - y], *oROW = oM[y];
            for (int x = 0; x < oW; x++) {
                oROW[x] = iROW[iW - 1 - x];
            }
            return;
        }
    }
    
    float** K = np_zero_matrix_float(5, 5, NULL );
    SEAMC_mk_kernel(K); // Could even be done once statically
            
    SEAMC_WORK_t WORK; // Consistent values across multiple SEAMC calls (rather than globals)
    
    int32_t* B = np_zero_array_int32(iH);
    
    int num_carveH = iW - oW, num_carveV = iH - oH;
    int disableTFJ = 0; // Not referenced elsewhere?
    
    float** O = np_zero_matrix_float(iH, iW, NULL );
    float** OO = np_zero_matrix_float(iH, iW, NULL );
    
    WORK.width = iW;
    WORK.height = iH;
    while (WORK.width > oW) {   // TODO: Deal with vertical too!!!
        // TODO: Track time
        WORK.start_time = time(NULL );
        WORK.start_clock = clock();
        WORK.ydim = WORK.height - 3;
        WORK.xdim = WORK.width - 3;
        WORK.yydim = WORK.height - 5;
        WORK.xxdim = WORK.width - 5;
        
        SEAMC_zeroKernel(O, oH, oW);
        SEAMC_zeroKernel(OO, oH, oW);
        
        SEAMC_tfj_conv2d(&WORK, iM, O, K);
        
        //SEAMC_padKernel(OO, WORK.height, WORK.width);
        for (int i = 0; i < WORK.height; i++) {
            for (int j = 0; j < 20; j++) {
                OO[i][j] = 1000000.0;
                OO[i][WORK.width - j - 1] = 1000000.0;
            }
        }

        SEAMC_dp(&WORK, OO, O);

        time_t t0 = time(NULL );
        SEAMC_backtrack(&WORK, OO, B);
        double secs = difftime(time(NULL ), t0);
        printf("%f sec in backtrack (c function)", secs);

        for (int y = WORK.yydim; y < WORK.height; y++) {
            B[y] = B[WORK.yydim - 1];
        }
        for (int y = 0; y < 2; y++) {
            B[y] = B[2];
        }
        // Something fishy here, seems like this should loop differently:
        SEAMC_copyKernel(&WORK, iM,WORK.width-1,B[2]); // Was B[i] and i was likely left at 2

        WORK.width -= 1;
        double elapsed = difftime(time(NULL) ,WORK.start_time);
        printf("%f sec this iteration", elapsed);

        //IF RETURNING fresh array: float **II = np_zero_matrix_float(WORK.height,WORK.width+1,NULL);

        for (int y = 0; y<WORK.height; y++) {
//        oM[i] = I[i][:(width+1)]; // Want up through and including width (one extra value)
        }
    }
    
} /* SOURCE:
 K = np.ones((5,5), dtype=np.float32); # I think the "ones" is not needed???
 B = np.zeros(height,dtype=np.int32);

 mk_kernel(K); # This probably writes to all 5x5 values???

 owidth = ???
 oheight = ???
 num_carve = ???;
 disableTFJ = ???;

 O = np.zeros((height,width),dtype=np.float32);
 OO = np.zeros((height,width),dtype=np.float32);

 while (width > (owidth - num_carve)):
 start = time.time();
 ydim = height-3;
 xdim = width-3;
 yydim = height-5;
 xxdim = width-5;
 
 zeroKernel(O,oheight,np.int32(width));
 zeroKernel(OO,oheight,np.int32(width));

 #using closures here...
 tfj_conv2d(I,O,K);

 #pad with 'float max'
 #padKernel(OO,np.int32(height),np.int32(width));
 for i in range(0,height):
 for j in range(0,20):
 OO[i][j] = 1000000.0;
 OO[i][width-j-1] = 1000000.0;
 
 #do dynamic programming
 dp(OO,O)

 #backtrack in python            
 #t0 = time.time();
 backtrack(OO,B)
 #t1 = time.time() - t0;
 #print '%f sec in backtrack (py function)' % t1

 for i in range(yydim,height):
 B[i] = B[yydim-1];
 for i in range(0, 2):
 B[i] = B[2];
 
 copyKernel(I,np.int32((width-1)),B[i]);

 width = width - 1;
 elapsed = (time.time() - start);
 print '%f sec per iteration' % elapsed

 II = np.zeros((height,(width+1)),dtype=np.float32);

 for i in range(0,height):
 II[i] = I[i][:(width+1)]
 */

