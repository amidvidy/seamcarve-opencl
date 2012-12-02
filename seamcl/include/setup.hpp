#ifndef SETUP_HPP
#define SETUP_HPP

// OpenCL
#include <CL/cl.hpp>

// Functions relating to configuring openCL objects
namespace setup {

    void args(int argc, char** argv,
              std::string &inputFile,
              std::string &outputFile,
              int &colsToRemove);

    /**
     * Creates an openCL context. The current preference is for a gpu context. In the future,
     * it would be nice to give the user the option to interactively select which context they
     * would like to use.
     * @return An openCL context object.
     */
    cl::Context context();

    /**
     * Creates an openCL command queue.
     * @param ctx An openCL context object.
     * @return An openCL command queue object.
     */
    cl::CommandQueue commandQueue(const cl::Context &ctx);

    /**
     * Creates an openCL kernel object. This actually involves three separate steps that can each fail,
     * reading the text of the file from the OS filesystem, creating an openCL program object, and finally
     * compiling the kernel for each available device.
     * @param ctx An openCL context object.
     * @param fileName The name of the file containing the kernel source.
     * @return An OpenCL kernel object.
     */
    cl::Kernel kernel(cl::Context &ctx, std::string fileName, std::string kernelFunc);

} // namespace setup

#endif
