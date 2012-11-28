#ifndef VERIFY_HPP
#define VERIFY_HPP

// C
#include <cmath>

// STL
#include <iostream>
#include <algorithm>

// OpencL
#include <CL/cl.hpp>

#define rM(M,X,Y) (M)[((X)*pitch+(Y))]

// Checks to ensure that kernels produce correct output
namespace verify {

    bool computeSeams(float* deviceResult,
                      float* originalEnergyMatrix,
                      int inset,
                      int width,
                      int height,
                      int pitch) {

        std::cerr << "in verify::computeSeams" << std::endl;
        float *hostResult = new float[width * height];
        std::cerr << "Before memset" << std::endl;
        memset(hostResult, 0, width * height * sizeof(float));
        std::cerr << "After memset" << std::endl;
        for (int x = inset; x < width - inset; ++x) {
            for (int y = 1; y < height; ++y) {
                rM(hostResult, x, y) = rM(originalEnergyMatrix, x, y) + std::min(rM(hostResult, x, y-1),
                                                                                 std::min(rM(hostResult, x-1, y-1),
                                                                                          rM(hostResult, x+1, y-1)));
            }
        }

        float epsilon = 0.00001f;
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                if (fabsf(rM(hostResult, x, y) - rM(deviceResult, x, y)) > epsilon) {
                    delete [] hostResult;
                    return false;
                }
            }
        }
        delete [] hostResult;
        return true;
    }

}

#endif
