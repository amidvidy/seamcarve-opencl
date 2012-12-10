#ifndef _NUMCY_H_
#define _NUMCY_H_

/* Couple "drop-in" replacements for some NumPy calls. */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static const bool DBG_DUMPIMG = false;
static const bool DBG_DUMPTXT = false;

struct I2_t {
	int x, y;

	inline I2_t()
			: x(0), y(0)
	{
	}
	inline I2_t(int iX, int iY)
			: x(iX), y(iY)
	{
	}
	inline I2_t(const I2_t &iI2)
			: x(iI2.x), y(iI2.y)
	{
	}
};
static inline I2_t operator+(const I2_t &A2, const int &B)
{
	return I2_t(A2.x + B, A2.y + B);
}
static inline I2_t operator-(const I2_t &A2, const int &B)
{
	return I2_t(A2.x - B, A2.y - B);
}
static inline I2_t operator*(const I2_t &A2, const int &B)
{
	return I2_t(A2.x * B, A2.y * B);
}
static inline I2_t operator/(const I2_t &A2, const int &B)
{
	return I2_t(A2.x / B, A2.y / B);
}
static inline I2_t operator+(const I2_t &A2, const I2_t &B2)
{
	return I2_t(A2.x + B2.x, A2.y + B2.y);
}
static inline I2_t operator-(const I2_t &A2, const I2_t &B2)
{
	return I2_t(A2.x - B2.x, A2.y - B2.y);
}

struct F4_t {
	float x, y, z, w;

	inline F4_t()
			: x(0.0f), y(0.0f), z(0.0f), w(0.0f)
	{
	}
	inline F4_t(float iX, float iY, float iZ, float iW)
			: x(iX), y(iY), z(iZ), w(iW)
	{
	}
	inline F4_t& operator+=(const F4_t &B4)
	{
		x += B4.x;
		y += B4.y;
		z += B4.z;
		w += B4.w;
		return *this;
	}
	inline F4_t& operator+=(const float &B)
	{
		x += B;
		y += B;
		z += B;
		w += B;
		return *this;
	}
	inline F4_t& operator*=(const float &B)
	{
		x *= B;
		y *= B;
		z *= B;
		w *= B;
		return *this;
	}
	inline F4_t& operator/=(const float &B)
	{
		x /= B;
		y /= B;
		z /= B;
		w /= B;
		return *this;
	}
};
static inline F4_t operator+(const F4_t &A4, const F4_t &B4)
{
	return F4_t(A4.x + B4.x, A4.y + B4.y, A4.z + B4.z, A4.w + B4.w);
}
static inline float dot4(const F4_t &A4, const F4_t &B4)
{
	return (A4.x * B4.x) + (A4.y * B4.y) + (A4.z * B4.z) + (A4.w * B4.w);
}
static inline F4_t operator*(const F4_t &A4, const float &B)
{
	return F4_t(A4.x * B, A4.y * B, A4.z * B, A4.w * B);
}
static inline float operator*(const F4_t &A4, const F4_t &B4)
{
	return dot4(A4, B4);
}

struct IMG4_t {
	const F4_t** PIX;
	int w, h;
	inline IMG4_t(const F4_t** &iPIX, int iW, int iH)
			: PIX(iPIX), w(iW), h(iH)
	{
	}
	inline const F4_t* getROW(int y) const // No range check
	{
		return PIX[y];
	}
};
static inline F4_t readImage4Clip(const IMG4_t &IMG4, int x, int y)
{
	const int clipX = (x < 0) ? 0 : (x >= IMG4.w) ? IMG4.w - 1 : x;
	const int clipY = (y < 0) ? 0 : (y >= IMG4.h) ? IMG4.h - 1 : y;
	return IMG4.getROW(clipY)[clipX]; // As array of ptr to array of F4_t
}
static inline F4_t readImage4Clip(const IMG4_t &IMG4, const I2_t &P)
{
	return readImage4Clip(IMG4, P.x, P.y);
}

/*
 ** Can turn into something else later, and matrix
 **   storage could change from current (convenient)
 **   c-style array of array's.
 ** An important thing to add is GPU style array strides.
 */

void DebugMatrix(void **IMG, int W, int H, const char* name, int remainWidth,
		bool isCOLOR);

inline void* np_new_array_x(size_t length, size_t sz, bool doZero = false)
{
	return (doZero) ? ::calloc(length * sz, 1) : ::malloc(length * sz); // calloc zeros for us
}
inline void* np_free_array_x(void* A)
{
	if (A) ::free(A);
	return NULL;
}
void** np_new_matrix_x(size_t height, size_t width, size_t *pPitch, size_t sz,
		bool doZero = false);
void** np_free_matrix_x(void** M);

template<class T>
inline T* np_new_array(size_t LEN)
{
	return (T*) np_new_array_x(LEN, sizeof(T), false);
}
template<class T>
inline T* np_zero_array(size_t LEN)
{
	return (T*) np_new_array_x(LEN, sizeof(T), true);
}
template<class T>
inline T** np_new_matrix(size_t H, size_t W, size_t *pP)
{
	return (T**) np_new_matrix_x(H, W, pP, sizeof(T), false);
}
template<class T>
inline T** np_zero_matrix(size_t H, size_t W, size_t *pP)
{
	return (T**) np_new_matrix_x(H, W, pP, sizeof(T), true);
}
template<class T>
inline T* np_free_array(T* A)
{
	return (T*) np_free_array_x((void*) A);
}
template<class T>
inline T** np_free_matrix(T** M)
{
	return (T**) np_free_matrix_x((void**) M);
}

#endif //_NUMCY_H_
