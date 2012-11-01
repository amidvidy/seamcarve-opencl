import Image
import ImageFilter
import sys
import numpy as np
import math
#from VecFunction import *
#from decorators import *
import time

def mk_kernel(K):
    s = 2.3;
    c0 = 1.0 / (2.0*math.pi*s*s);
    c1 = -1.0 / (2.0*s*s);
    for y in range(0,5):
        ym = y - 2;
        for x in range(0,5):
            xm = x - 2;
            K[y][x] = c0 * math.exp((xm*xm + ym*ym)*c1);

#@vec
def tfj_conv2d(I,O,K):
    for y in range(3,ydim):
        for x in range(3,xdim):
            for yy in range(-2,3):
                for xx in range(-2, 3):
                    O[y][x] = O[y][x] + K[2+yy][2+xx] * I[y+yy][x+xx];

#@vec
def dp(Y,G):
    for i in range(5,yydim):
        for j in range(5,xxdim):
            Y[i][j] = G[i][j] + min(min(Y[i-1][j-1], Y[i-1][j]),Y[i-1][j+1]);


#@vec
def copyKernel(I,width_m1,c):
    for i in range(0,height):
        for j in range(c, width_m1):
            I[i][j] = I[i][j+1]

#@vec
def zeroKernel(Y,h,w):
    for i in range(0,h):
        for j in range(0,w):
            Y[i][j] = 0

def padKernel(OO,h,w):
    for i in range(0,h):
        for j in range(0,20):
            OO[i][j] = 1000000.0;
            OO[i][w-j-1] = 1000000.0;

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


global ext
ext = ".jpg"

if(len(sys.argv) < 2):
    print 'no image file :('
    quit()

imageFile = sys.argv[1]
im1 = Image.open(imageFile);
width,height = im1.size
im1 = im1.convert('L');


print 'width=%d' % width
print 'height=%d' % height

I = np.asarray(im1,dtype=np.float32);
K = np.ones((5,5), dtype=np.float32);
B = np.zeros(height,dtype=np.int32);

mk_kernel(K);

owidth = np.int32(width);
oheight = np.int32(height);

num_carve = 10;
disableTFJ = False;


if(len(sys.argv) == 3):
    num_carve = int(sys.argv[2]);
elif(len(sys.argv) == 4):
    num_carve = int(sys.argv[2]);
    disableTFJ = (sys.argv[3] == 'True');

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
    
im2 = Image.fromarray(II);
im2.show()
print 'done'
