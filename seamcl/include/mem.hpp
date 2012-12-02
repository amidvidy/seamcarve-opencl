#ifndef MEM_HPP
#define MEM_HPP

// OpenCL
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

// STL
#include <iostream>

// Functions relating to allocating device memory, and marshalling data back and forth from the device.
namespace mem {

    cl::Buffer buffer(cl::Context &ctx,
                      cl::CommandQueue &cmdQueue,
                      int numBytes);

    template<typename T>
    void read(cl::Context &ctx, cl::CommandQueue &cmdQueue, T *arr, cl::Buffer &buff, size_t numBytes) {
        std::cout << "numBytes trying to read = " << numBytes << std::endl;
        cl_int errNum = cmdQueue.enqueueReadBuffer(buff,
                                                   CL_TRUE,
                                                   0,
                                                   numBytes,
                                                   arr,
                                                   NULL,
                                                   NULL);
        if (errNum != CL_SUCCESS) {
            std::cerr << "Error reading buffer from device to host." << std::endl;
            exit(-1);
        }
    }

    // This only works for stack allocated arrays when their size can be fully determined at compile time.
    template<typename T, std::size_t sz>
    void read(cl::Context &ctx, cl::CommandQueue &cmdQueue, T(&arr)[sz], cl::Buffer &buff) {
        cl_int errNum = cmdQueue.enqueueReadBuffer(buff,
                                                   CL_TRUE,
                                                   0,
                                                   sz * sizeof(T),
                                                   arr,
                                                   NULL,
                                                   NULL);
        if (errNum != CL_SUCCESS) {
            std::cerr << "Error reading buffer from device to host." << std::endl;
            exit(-1);
        }
    }

    // Same as above.
    template<typename T, std::size_t sz>
    void write(cl::Context &ctx, cl::CommandQueue &cmdQueue, T(&arr)[sz], cl::Buffer &buff) {
        cl_int errNum = cmdQueue.enqueueWriteBuffer(buff,
                                                    CL_TRUE,
                                                    0,
                                                    sz * sizeof(T),
                                                    (void *) arr,
                                                    NULL,
                                                    NULL);
        if (errNum != CL_SUCCESS) {
            std::cerr << "Error writing buffer from host to device." << std::endl;
            exit(-1);
        }
    }

    template<typename T>
    void write(cl::Context &ctx, cl::CommandQueue &cmdQueue, T *arr, cl::Buffer &buff, size_t numBytes) {
        std::cout << "numBytes trying to write = " << numBytes << std::endl;
        cl_int errNum = cmdQueue.enqueueWriteBuffer(buff,
                                                    CL_TRUE,
                                                    0,
                                                    numBytes,
                                                    arr,
                                                    NULL,
                                                    NULL);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error writing buffer from host to device." << std::endl;
            exit(-1);
        }
    }

} // namespace mem

#endif
