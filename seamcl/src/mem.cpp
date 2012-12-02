#include "mem.hpp"

#include <iostream>

// Functions relating to allocating device memory, and marshalling data back and forth from the device.
namespace mem {

    cl::Buffer buffer(cl::Context &ctx,
                      cl::CommandQueue &cmdQueue,
                      int numBytes) {
        cl_int errNum;
        cl::Buffer buff = cl::Buffer(ctx,
                                     CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                     numBytes,
                                     NULL,
                                     &errNum);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error allocating a buffer on the device." << std::endl;
            std::cerr << "numBytes = " << numBytes << std::endl;
            std::cerr << "errNum = " << errNum << std::endl;
            exit(-1);
        }

        return buff;
    }

} // namespace mem
