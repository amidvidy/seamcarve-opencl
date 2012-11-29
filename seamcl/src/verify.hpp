#ifndef VERIFY_HPP
#define VERIFY_HPP

// C
#include <cmath>

// STL
#include <iostream>
#include <algorithm>
#include <limits>

// OpencL
#include <CL/cl.hpp>

#define rM(M,X,Y) (M)[((Y)*pitch+(X))]
#define min3(A,B,C) (std::min((A),std::min((B),(C))))
#define pROW(M,Y) (M)+((Y)*pitch)

// Checks to ensure that kernels produce correct output
namespace verify {

    inline void printMatrix(float *matrix, int height, int width, int pitch) {

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                std::cout << rM(matrix, i, j) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }



    bool computeSeams(float* deviceResult,
                      float* originalEnergyMatrix,
                      int width,
                      int height,
                      int pitch) {


        std::cerr << "in verify::computeSeams" << std::endl;
        float *hostResult = new float[pitch * height];
        memcpy(hostResult, originalEnergyMatrix, pitch * height * sizeof(float));

        for (int y = 1; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                rM(hostResult, x, y) = rM(originalEnergyMatrix, x, y) + std::min(rM(hostResult,                   x, y-1),
                                                                        std::min(rM(hostResult,    std::max(x-1, 0), y-1),
                                                                                 rM(hostResult, std::min(x+1,width), y-1)));
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
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                if (fabsf(rM(hostResult, x, y) - rM(deviceResult, x, y)) > epsilon) {
                    //std::cerr << "Mismatch at (" << x << ", " << y << ") " << std::endl;
                    //std::cerr << "Expected:\t" << rM(hostResult, x, y) << std::endl;
                    //std::cerr << "Actual:\t" << rM(deviceResult, x, y) << std::endl;
                    correct = false;
                }
            }
        }
        delete [] hostResult;
        std::cout << "finished verifying computeSeams." << std::endl;
        return correct;
    }

    bool backtrack(int* deviceResult,
                   float* energyMatrix,
                   int width,
                   int height,
                   int pitch) {
        std::cerr << "in verify::backtrack" << std::endl;
        float *hostResult = new float[height];

        int y = height - 1;
        int carveX = 1;

        float *ROW = pROW(energyMatrix, y);
        int x = carveX;
        float min_v = ROW[x];
        while (++x < (width-1)) {
            if (ROW[x] < min_v) {
                min_v = ROW[x];
                carveX = x;
            }
        }
        hostResult[y] = carveX;
        while (y >= 0) {
            ROW = pROW(energyMatrix, y);
            const float L = (carveX < 1) ? std::numeric_limits<float>::max() : ROW[carveX-1];
            const float C = ROW[carveX];
            const float R = (carveX >= width) ? std::numeric_limits<float>::max() : ROW[carveX+1];
            if (L < C) {
                carveX += (L < R) ? -1 : 1;
            } else {
                carveX += (C < R) ? 0 : 1;
            }
            hostResult[y] = carveX;
        }

        //Check arrays are equal
        for (int i = 0; i < height; ++i) {
            if (hostResult[i] != deviceResult[i]) {
                delete [] hostResult;
                return false;
            }
        }
        delete [] hostResult;
        return true;
    }


}

#endif
