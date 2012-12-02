#ifndef VERIFY_HPP
#define VERIFY_HPP

// OpenCL
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

// STL
#include <iostream>

using namespace std;

typedef long long int64;
typedef unsigned long long uint64;

// Testing utilities
namespace verify {

	bool computeSeams(float* deviceResult,
					  float* originalEnergyMatrix,
					  int width,
					  int height,
					  int pitch,
					  int colsRemoved);

	bool testMemReadWrite(cl::Context &ctx, cl::CommandQueue &cmdQueue, int numInts);

    uint64 timeMillis();

    //#define min3(A,B,C) (min((A),min((B),(C))))
    template <typename T>
    inline const T& min3(const T &A, const T &B, const T &C) {
    	return min(A, min(B, C));
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
                cout << rM(matrix, pitch, i, j) << "  \t";
            }
            cout << endl;
        }
        cout << endl;
    }
}

#endif
