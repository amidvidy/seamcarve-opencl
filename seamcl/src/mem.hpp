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


} // namespace mem {

#endif
