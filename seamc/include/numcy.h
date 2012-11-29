#ifndef _NUMCY_H_
#define _NUMCY_H_

/* Couple "drop-in" replacements for some NumPy calls. */

#include <stddef.h>
#include <stdint.h>

struct F4_t {
    float x, y, z, w;
    
    inline F4_t(): x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    inline F4_t(float iX, float iY, float iZ, float iW):
        x(iX), y(iY), z(iZ), w(iW) {}
};

inline float dot4(const F4_t &A4, const F4_t &B4) {
    return (A4.x * B4.x) + (A4.y * B4.y) + (A4.z * B4.z) + (A4.w * B4.w);
}

inline F4_t readImage4Clip(const F4_t **IMG4, int x, int y, int width, int height) {
    const int clipX = (x<0) ? 0 : (x>width ) ? width  : x;
    const int clipY = (y<0) ? 0 : (y>height) ? height : y;
    return IMG4[clipY][clipX]; // As array of ptr to array of F4_t
}

/*
 ** Can turn into something else later, and matrix
 **   storage could change from current (convenient)
 **   c-style array of array's.
 ** An important thing to add is GPU style array strides.
 */

int32_t* np_zero_array_int32(size_t length);
int32_t** np_zero_matrix_int32(size_t height, size_t width, size_t *pPitch);
int32_t* np_free_array_int32(int32_t* A);
int32_t** np_free_matrix_int32(int32_t** M);

/* WARNING: These just cheat and cast int32 to float (usually 32-bits too) */
float* np_zero_array_float(size_t length);
float** np_zero_matrix_float(size_t height, size_t width, size_t *pPitch);
float* np_free_array_float(float* A);
float** np_free_matrix_float(float** M);

#endif //_NUMCY_H_
