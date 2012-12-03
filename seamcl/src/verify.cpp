#include "verify.hpp"

// C
#include <cmath>
#include <sys/time.h>
#include <ctime>

// STL
#include <iostream>
#include <algorithm>
#include <limits>

// Checks to ensure that kernels produce correct output
namespace verify {

    uint64 timeMillis() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        uint64 ret = tv.tv_usec;
        ret /= 1000;
        ret += (tv.tv_sec * 1000);
        return ret;
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
                rM(hostResult, pitch, x, y) = rM(originalEnergyMatrix, pitch, x, y) +
                    std::min(rM(hostResult, pitch, x, y-1),
                             std::min(rM(hostResult, pitch, std::max(x-1, 0), y-1),
                                      rM(hostResult, pitch, std::min(x+1,imgEndIdx), y-1)));
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
                if (fabs(rM(hostResult, pitch, x, y) - rM(deviceResult, pitch, x, y)) > epsilon) {
                    std::cerr << "Mismatch at (" << x << ", " << y << ") " << std::endl;
                    std::cerr << "Expected:\t" << rM(hostResult, pitch, x, y) << std::endl;
                    std::cerr << "Actual:\t" << rM(deviceResult, pitch, x, y) << std::endl;
                    correct = false;
                }
            }
        }
        delete [] hostResult;
        std::cout << "finished verifying computeSeams." << std::endl;
        return correct;
    }

} // namespace verify
