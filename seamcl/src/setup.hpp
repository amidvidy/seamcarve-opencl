#ifndef SETUP_HPP
#define SETUP_HPP

// OpenCL
#include <CL/cl.hpp>

// Functions relating to configuring openCL objects
namespace setup {

    void args(int argc, char** argv,
              std::string &inputFile,
              std::string &outputFile,
              int &desiredWidth,
              int &desiredHeight) {
        if (argc < 5) {
            std::cerr << "USAGE: seamcl <INPUT> <OUTPUT> <DESIRED WIDTH> <DESIRED HEIGHT>" << std::endl;
            exit(-1);
        }

        // Parse arguments
        inputFile = std::string(argv[1]);
        outputFile = std::string(argv[2]);

        std::istringstream s1(argv[3]);
        std::istringstream s2(argv[4]);
        if (! (s1 >> desiredWidth)) {
            std::cerr << "DESIRED WIDTH must be an integer." << std::endl;
            exit(-1);
        } else if (! (s2 >> desiredHeight)) {
            std::cerr << "DESIRED HEIGHT must be an integer." << std::endl;
            exit(-1);
        }

    }


    // TODO(amidvidy): error handling

    /**
     * Creates an openCL context. The current preference is for a gpu context. In the future,
     * it would be nice to give the user the option to interactively select which context they
     * would like to use.
     * @return An openCL context object.
     */
    cl::Context context() {
        std::vector<cl::Platform> platformList;
        cl::Platform::get(&platformList);

        cl_context_properties cprops[] = {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties)(platformList[0])(),
            0
        };
        // OK since cl objects are ref counted.
        return cl::Context(CL_DEVICE_TYPE_GPU, cprops);
    }

    // TODO(amidvidy): error handling

    /**
     * Creates an openCL command queue.
     * @param ctx An openCL context object.
     * @return An openCL command queue object.
     */
    cl::CommandQueue commandQueue(const cl::Context &ctx) {
      std::vector<cl::Device> devices;
      ctx.getInfo((cl_context_info)CL_CONTEXT_DEVICES, &devices);
      //std::vector<cl::Device> devices = ctx.getInfo<CL_CONTEXT_DEVICES>();

        // DEBUGGING
        for (size_t i = 0; i < devices.size(); ++i) {
            cl::Device &device = devices[i];
	    std::string deviceName; device.getInfo((cl_device_info)CL_DEVICE_NAME, &deviceName);
	    std::string deviceVendor; device.getInfo((cl_device_info)CL_DEVICE_VENDOR, &deviceVendor);
	    std::string deviceProfile; device.getInfo((cl_device_info)CL_DEVICE_PROFILE, &deviceProfile);
	    std::string deviceVersion; device.getInfo((cl_device_info)CL_DEVICE_VERSION, &deviceVersion);
	    std::string driverVersion; device.getInfo((cl_device_info)CL_DRIVER_VERSION, &driverVersion);
	    std::string deviceCLVersion; device.getInfo((cl_device_info)CL_DEVICE_OPENCL_C_VERSION, &deviceCLVersion);
	    std::string deviceExtensions; device.getInfo((cl_device_info)CL_DEVICE_EXTENSIONS, &deviceExtensions);

            std::cout << "Info for device #" << 0 << ":" << std::endl;
            std::cout << "\tName:\t"
                      << deviceName << std::endl;
            std::cout << "\tVendor:\t"
                      << deviceVendor << std::endl;
            std::cout << "\tProfile:\t"
                      << deviceProfile << std::endl;
            std::cout << "\tDevice Version:\t"
                      << deviceVersion << std::endl;
            std::cout << "\tDriver Version:\t"
                      << driverVersion << std::endl;
            std::cout << "\tDevice OpenCL C Version:\t"
                      << deviceCLVersion << std::endl;
            std::cout << "\tDevice Extensions:\t"
                      << deviceExtensions << std::endl;
        }

        return cl::CommandQueue(ctx, devices[0], CL_QUEUE_PROFILING_ENABLE);
    }

    /**
     * Creates an openCL kernel object. This actually involves three separate steps that can each fail,
     * reading the text of the file from the OS filesystem, creating an openCL program object, and finally
     * compiling the kernel for each available device.
     * @param ctx An openCL context object.
     * @param fileName The name of the file containing the kernel source.
     * @return An OpenCL kernel object.
     */
    cl::Kernel kernel(cl::Context &ctx, std::string fileName, std::string kernelFunc) {
        std::ifstream file(fileName.c_str());
        std::string programText((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());

        cl::Program::Sources sources;
        // add one to size for null terminator
        sources.push_back(std::make_pair(programText.c_str(), programText.size() + 1));

        cl_int errNum;

        cl::Program program = cl::Program(ctx,
                                          sources,
                                          &errNum);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Failed to create CL program from source." << std::endl;
            exit(-1);
        }

        std::vector<cl::Device> devices = ctx.getInfo<CL_CONTEXT_DEVICES>();
        // TODO(amidvidy): figure out how to reuse the device vector we created earlier?
        errNum = program.build(ctx.getInfo<CL_CONTEXT_DEVICES>());

        if (errNum != CL_SUCCESS) {
            std::cerr << "Failed to compile program." << std::endl;
            std::string buildLog;
            program.getBuildInfo(devices[0], (cl_program_build_info) CL_PROGRAM_BUILD_LOG, &buildLog);
            std::cerr << buildLog;
            exit(-1);
        }


        cl::Kernel kernel = cl::Kernel(program, kernelFunc.c_str(), &errNum);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Failed to create kernel" << std::endl;
            exit(-1);
        }

        return kernel;

    }


} // namespace setup {

#endif
