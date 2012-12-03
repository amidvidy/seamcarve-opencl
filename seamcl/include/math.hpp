#ifndef MATH_HPP
#define MATH_HPP

#include <string>

// Math utils
namespace math {

    /**
     * Utility function to round up to the nearest multiple of the group size.
     * @param groupSize The work group size.
     * @param globalSize The unrounded globalSize
     * @return The smallest multiple of groupSize greater than globalSize.
     */
    inline size_t roundUp(int groupSize, int globalSize) {
        int r = globalSize % groupSize;
        if (r == 0) {
            return globalSize;
        } else {
            return globalSize + groupSize - r;
        }
    }

} // namespace math

#endif
