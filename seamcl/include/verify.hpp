#ifndef VERIFY_HPP
#define VERIFY_HPP

// OpenCL
#include "ocl.hpp"

// C
#include <cmath>
#include <sys/time.h>
#include <ctime>


// STL
#include <iostream>
#include <algorithm>
#include <limits>

typedef long long int64;
typedef unsigned long long uint64;

// Checks to ensure that kernels produce correct output
namespace verify {

	uint64 timeMillis();

	bool computeSeams(float* deviceResult,
                  float* originalEnergyMatrix,
                  int width,
                  int height,
                  int pitch,
                  int colsRemoved);

	template<typename T>
    bool arraysEqual(T *&a, T *&b, size_t numElems) {
        for (size_t i = 0; i < numElems; ++i) {
            if (a[i] != b[i]) {
                return false;
            }
        }
        return true;
    }

    //#define min3(A,B,C) (min((A),min((B),(C))))
    template <typename T>
    inline const T& min3(const T &A, const T &B, const T &C) {
    	return std::min(A, std::min(B, C));
    }

	// #define rM(M,X,Y) (M)[((Y)*pitch+(X))]
    template <typename T>
    inline T& rM(T *M, int pitch, int X, int Y) {
    	return M[(Y * pitch) + X];
    }
    template <typename T>
    inline const T& rM(const T *M, int pitch, int X, int Y) {
    	return M[(Y * pitch) + X];
    }

	//#define pROW(M,Y) (M)+((Y)*pitch)
    template <typename T>
    inline T*& pROW(T *M, int pitch, int Y) {
    	return M + (Y * pitch);
    }
    template <typename T>
    inline const T*& pROW(const T *M, int pitch, int Y) {
    	return M + (Y * pitch);
    }

    template <typename T>
    void printMatrix(T *matrix, int height, int width, int pitch) {
        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                std::cout << rM<T>(matrix, pitch, i, j) << "  \t";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

} // namespace verify

#endif // VERIFY_HPP
