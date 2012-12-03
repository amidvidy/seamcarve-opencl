#ifndef OCL_HPP_
#define OCL_HPP_

#ifndef __NO_STD_VECTOR
#define __NO_STD_VECTOR // Use cl::vector instead of STL version
#endif

#ifndef __CL_ENABLE_EXCEPTIONS
#define __CL_ENABLE_EXCEPTIONS
#endif

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#endif /* OCL_HPP_ */
