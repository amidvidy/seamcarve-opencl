#ifndef VERIFY_HPP
#define VERIFY_HPP

// C
#include <cmath>
#include <sys/time.h>
#include <ctime>


// STL
#include <iostream>
#include <algorithm>
#include <limits>
#include <random>

// OpencL
#include <CL/cl.hpp>

#define rM(M,X,Y) (M)[((Y)*pitch+(X))]
#define min3(A,B,C) (std::min((A),std::min((B),(C))))
#define pROW(M,Y) (M)+((Y)*pitch)

typedef long long int64;
typedef unsigned long long uint64;

// Testing utilities
namespace verify {

    bool testMemReadWrite(cl::Context &ctx, cl::CommandQueue &cmdQueue, int numInts) {
        int *randNums = new int[numInts];
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(1, 10000);
        for (int i = 0; i < numInts; ++i) {
            randNums[i] = distribution(generator);
        }
        // Create a buffer on device
        cl::Buffer deviceBuff = mem::buffer(ctx, cmdQueue, numInts * sizeof(int));
        // Copy ints over
        mem::write(ctx, cmdQueue, randNums, deviceBuff, numInts * sizeof(int));
        // Copy ints back
        int *hostBuff = new int[numInts];
        mem::read(ctx, cmdQueue, hostBuff, deviceBuff, numInts * sizeof(int));
        // Check equality
        bool passed = true;
        for (int i = 0; i < numInts; ++i) {
            if (randNums[i] != hostBuff[i]) {
                std::cerr << "TEST MEM READ AND WRITE FAILED!!!" << std::endl;
                passed = false;
                break;
            }
        }
        delete [] randNums;
        delete [] hostBuff;
        return passed;
    }

    uint64 timeMillis() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        uint64 ret = tv.tv_usec;
        ret /= 1000;
        ret += (tv.tv_sec * 1000);
        return ret;
    }

    inline void printMatrix(float *matrix, int height, int width, int pitch) {

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
                      int colsRemoved) {


        std::cerr << "in verify::computeSeams" << std::endl;
        float *hostResult = new float[pitch * height];
        memcpy(hostResult, originalEnergyMatrix, pitch * height * sizeof(float));

        const int imgEndIdx = width - colsRemoved;

        for (int y = 1; y < height; ++y) {
            for (int x = 0; x < imgEndIdx; ++x) {
                rM(hostResult, x, y) = rM(originalEnergyMatrix, x, y) +
                    std::min(rM(hostResult,x, y-1),
                             std::min(rM(hostResult, std::max(x-1, 0), y-1),
                                      rM(hostResult, std::min(x+1,imgEndIdx), y-1)));
            }
        }

        std::cerr << "height\t" << height << std::endl;
        std::cerr << "width\t" << width << std::endl;


        //print original matrix

        // std::cout << "ENERGYMATRIX: " << std::endl;
        // printMatrix(originalEnergyMatrix, height, width, pitch);

        // std::cout << "DEVICERESULT: " << std::endl;
        // printMatrix(deviceResult, height, width, pitch);

        // std::cout << "HOSTRESULT: " << std::endl;
        // printMatrix(hostResult, height, width, pitch);

        bool correct = true;

        float epsilon = 0.00001f;
        for (int x = 0; x < imgEndIdx; ++x) {
            for (int y = 0; y < height; ++y) {
                if (fabsf(rM(hostResult, x, y) - rM(deviceResult, x, y)) > epsilon) {
                    std::cerr << "Mismatch at (" << x << ", " << y << ") " << std::endl;
                    std::cerr << "Expected:\t" << rM(hostResult, x, y) << std::endl;
                    std::cerr << "Actual:\t" << rM(deviceResult, x, y) << std::endl;
                    correct = false;
                }
            }
        }
        delete [] hostResult;
        std::cout << "finished verifying computeSeams." << std::endl;
        return correct;
    }

}

#endif
