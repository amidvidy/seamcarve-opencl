#ifndef _NUMCY_H_
#define _NUMCY_H_

/* Couple "drop-in" replacements for some NumPy calls. */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static const bool DBG_DUMPIMG = false;
static const bool DBG_DUMPTXT = false;

struct I2_t {
    int x, y;

    inline I2_t():x(0),y(0) {}
    inline I2_t(int iX, int iY): x(iX), y(iY) {}
    inline I2_t(const I2_t &iI2): x(iI2.x), y(iI2.y) {}
};
static inline I2_t operator+(const I2_t &A2, const int &B) {
    return I2_t(A2.x + B, A2.y + B);
}
static inline I2_t operator-(const I2_t &A2, const int &B) {
    return I2_t(A2.x - B, A2.y - B);
}
static inline I2_t operator*(const I2_t &A2, const int &B) {
    return I2_t(A2.x * B, A2.y * B);
}
static inline I2_t operator/(const I2_t &A2, const int &B) {
    return I2_t(A2.x / B, A2.y / B);
}
static inline I2_t operator+(const I2_t &A2, const I2_t &B2) {
    return I2_t(A2.x + B2.x, A2.y + B2.y);
}
static inline I2_t operator-(const I2_t &A2, const I2_t &B2) {
    return I2_t(A2.x - B2.x, A2.y - B2.y);
}

struct F4_t {
    float x, y, z, w;
    
    inline F4_t(): x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    inline F4_t(float iX, float iY, float iZ, float iW):
        x(iX), y(iY), z(iZ), w(iW) {}
    inline F4_t& operator+=(const F4_t &B4) {
        x+=B4.x; y+=B4.y; z+=B4.z; w+=B4.w;
        return *this;
    }
    inline F4_t& operator+=(const float &B) {
        x+=B; y+=B; z+=B; w+=B;
        return *this;
    }
    inline F4_t& operator*=(const float &B) {
        x*=B; y*=B; z*=B; w*=B;
        return *this;
    }
    inline F4_t& operator/=(const float &B) {
        x/=B; y/=B; z/=B; w/=B;
        return *this;
    }
};
static inline F4_t operator+(const F4_t &A4, const F4_t &B4) {
    return F4_t(A4.x + B4.x, A4.y + B4.y, A4.z+B4.z, A4.w+B4.w);
}
static inline float dot4(const F4_t &A4, const F4_t &B4) {
    return (A4.x * B4.x) + (A4.y * B4.y) + (A4.z * B4.z) + (A4.w * B4.w);
}
static inline F4_t operator*(const F4_t &A4, const float &B) {
    return F4_t(A4.x * B, A4.y * B, A4.z * B, A4.w * B);
}
static inline float operator*(const F4_t &A4, const F4_t &B4) {
    return dot4(A4, B4);
}

struct IMG4_t {
    const F4_t** PIX;
    int w, h;
    inline IMG4_t(const F4_t** &iPIX, int iW, int iH):
            PIX(iPIX), w(iW), h(iH) {}
    inline const F4_t* getROW(int y) const // No range check
    { return PIX[y]; }
};
static inline F4_t readImage4Clip(const IMG4_t &IMG4, int x, int y) {
    const int clipX = (x<0) ? 0 : (x>=IMG4.w ) ? IMG4.w-1  : x;
    const int clipY = (y<0) ? 0 : (y>=IMG4.h) ? IMG4.h-1: y;
    return IMG4.getROW(clipY)[clipX]; // As array of ptr to array of F4_t
}
static inline F4_t readImage4Clip(const IMG4_t &IMG4, const I2_t &P) {
    return readImage4Clip(IMG4, P.x, P.y);
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

void DebugMatrix(void **IMG, int W, int H, const char* name, int remainWidth, bool isCOLOR);

#endif //_NUMCY_H_
