//This code works on CUDA GPU also (limited to OpenCL 1.1)
// From http://www.thebigblob.com/using-the-cpp-bindings-for-opencl/

#define __NO_STD_VECTOR // Use cl::vector instead of STL version
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include <utility>
#include <iostream>
#include <fstream>
#include <string>
using namespace cl;
 
int main() {
    // Create the two input vectors
    const int LIST_SIZE = 1000;
    int *A = new int[LIST_SIZE]; 
    int *B = new int[LIST_SIZE];
    for(int i = 0; i < LIST_SIZE; i++) {
        A[i] = i;
        B[i] = LIST_SIZE - i;
    }
 
   try { 
        // Get available platforms
        vector<Platform> platforms;
        Platform::get(&platforms);
 
        // Select the default platform and create a context using this platform and the GPU
        cl_context_properties cps[3] = { 
            CL_CONTEXT_PLATFORM, 
            (cl_context_properties)(platforms[0])(), 
            0 
        };
        Context context( CL_DEVICE_TYPE_GPU, cps);
 
        // Get a list of devices on this platform
        vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
 
        // Create a command queue and use the first device
        CommandQueue queue = CommandQueue(context, devices[0]);
 
        // Read source file
        std::ifstream sourceFile("vector_add_kernel.cl");
        std::string sourceCode(
            std::istreambuf_iterator<char>(sourceFile),
            (std::istreambuf_iterator<char>()));
        Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
 
        // Make program of the source code in the context
        Program program = Program(context, source);
 
        // Build program for these specific devices
        program.build(devices);
 
        // Make kernel
        Kernel kernel(program, "vector_add");
 
        // Create memory buffers
        Buffer bufferA = Buffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int));
        Buffer bufferB = Buffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int));
        Buffer bufferC = Buffer(context, CL_MEM_WRITE_ONLY, LIST_SIZE * sizeof(int));
 
        // Copy lists A and B to the memory buffers
        queue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, LIST_SIZE * sizeof(int), A);
        queue.enqueueWriteBuffer(bufferB, CL_TRUE, 0, LIST_SIZE * sizeof(int), B);
 
        // Set arguments to kernel
        kernel.setArg(0, bufferA);
        kernel.setArg(1, bufferB);
        kernel.setArg(2, bufferC);
 
        // Run the kernel on specific ND range
        NDRange global(LIST_SIZE);
        NDRange local(1);
        queue.enqueueNDRangeKernel(kernel, NullRange, global, local);
 
        // Read buffer C into a local list
        int *C = new int[LIST_SIZE];
        queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, LIST_SIZE * sizeof(int), C);
 
        for(int i = 0; i < LIST_SIZE; i ++)
             std::cout << A[i] << " + " << B[i] << " = " << C[i] << std::endl; 
    } catch(Error error) {
       std::cout << error.what() << "(" << error.err() << ")" << std::endl;
    }
 
    return 0;
}

