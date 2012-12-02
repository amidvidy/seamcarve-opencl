#ifndef VERIFY_HPP
#define VERIFY_HPP

// OpencL
#include <CL/cl.hpp>

#include <iostream>

#define rM(M,X,Y) (M)[((Y)*pitch+(X))]
#define min3(A,B,C) (std::min((A),std::min((B),(C))))
#define pROW(M,Y) (M)+((Y)*pitch)

typedef long long int64;
typedef unsigned long long uint64;

// Testing utilities
namespace verify {

    bool testMemReadWrite(cl::Context &ctx, cl::CommandQueue &cmdQueue, int numInts);

    uint64 timeMillis();

    inline
    void printMatrix(float *matrix, int height, int width, int pitch) {

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                std::cout << rM(matrix, i, j) << "  \t";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    bool computeSeams(float* deviceResult,
                      float* originalEnergyMatrix,
                      int width,
                      int height,
                      int pitch,
                      int colsRemoved);

}

#endif
