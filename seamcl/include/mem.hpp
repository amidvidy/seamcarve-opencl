#ifndef MEM_HPP
#define MEM_HPP

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
            exit(-1);
        }

        return buff;
    }

    template<typename T>
    cl::Buffer bufferFromHostArray(cl::Context &ctx,
                                   cl::CommandQueue &cmdQueue,
                                   T* data,
                                   size_t numBytes) {
        cl_int errNum;
        cl::Buffer buff = cl::Buffer(ctx,
                                     CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                     numBytes,
                                     data,
                                     &errNum);
        cmdQueue.flush(); cmdQueue.finish(); // Ensure that this finishes before returning.
        if (errNum != CL_SUCCESS) {
            std::cerr << "Error allocating device buffer from host array." << std::endl;
            exit(-1);
        }
        return buff;
    }


    template<typename T>
    void read(cl::Context &ctx, cl::CommandQueue &cmdQueue, T *arr, cl::Buffer &buff, size_t size) {
        cl_int errNum = cmdQueue.enqueueReadBuffer(buff,
                                                   CL_TRUE,
                                                   0,
                                                   size * sizeof(T),
                                                   (void *) arr,
                                                   NULL,
                                                   NULL);
        if (errNum != CL_SUCCESS) {
            std::cerr << "Error reading buffer from device to host." << std::endl;
            exit(-1);
        }
    }

    // This only works for stack allocated arrays.
    template<typename T, std::size_t sz>
    void read(cl::Context &ctx, cl::CommandQueue &cmdQueue, T(&arr)[sz], cl::Buffer &buff) {
        cl_int errNum = cmdQueue.enqueueReadBuffer(buff,
                                                   CL_TRUE,
                                                   0,
                                                   sz * sizeof(T),
                                                   (void *) arr,
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
            std::cerr << "Error writing buffer from device to host." << std::endl;
            exit(-1);
        }
    }

    // Same as above.
    template<typename T>
    void write(cl::Context &ctx, cl::CommandQueue &cmdQueue, T* arr, cl::Buffer &buff, size_t sz) {
        cl_int errNum = cmdQueue.enqueueWriteBuffer(buff,
                                                    CL_TRUE,
                                                    0,
                                                    sz * sizeof(T),
                                                    (void *) arr,
                                                    NULL,
                                                    NULL);
        if (errNum != CL_SUCCESS) {
            std::cerr << "Error writing buffer from device to host." << std::endl;
            exit(-1);
        }
    }

    

} // namespace mem {

#endif
