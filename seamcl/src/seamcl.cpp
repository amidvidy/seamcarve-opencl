// C
#include <cstdlib>

// STL
#include <fstream>
#include <iostream>
#include <string>
#include <streambuf>

// OpenCL
#include <CL/cl.hpp>

// FreeImage
#include "FreeImage.h"

// Functions relating to configuring openCL objects
namespace setup {

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
        std::vector<cl::Device> devices = ctx.getInfo<CL_CONTEXT_DEVICES>();

        // DEBUGGING
        for (int i = 0; i < devices.size(); ++i) {
            cl::Device &device = devices[i];
            std::cout << "Info for device #" << 0 << ":" << std::endl;
            std::cout << "\tName:\t"
                      << device.getInfo<CL_DEVICE_NAME>() << std::endl;
            std::cout << "\tVendor:\t"
                      << device.getInfo<CL_DEVICE_VENDOR>() << std::endl;
            std::cout << "\tProfile:\t"
                      << device.getInfo<CL_DEVICE_PROFILE>() << std::endl;
            std::cout << "\tDevice Version:\t"
                      << device.getInfo<CL_DEVICE_VERSION>() << std::endl;
            std::cout << "\tDriver Version:\t"
                      << device.getInfo<CL_DRIVER_VERSION>() << std::endl;
            std::cout << "\tDevice OpenCL C Version:\t"
                      << device.getInfo<CL_DEVICE_OPENCL_C_VERSION>() << std::endl;
            std::cout << "\tDevice Extensions:\t"
                      << device.getInfo<CL_DEVICE_EXTENSIONS>() << std::endl;
        }

        return cl::CommandQueue(ctx, devices[0]);
    }

    /**
     * Creates an openCL kernel object. This actually involves three separate steps that can each fail,
     * reading the text of the file from the OS filesystem, creating an openCL program object, and finally
     * compiling the kernel for each available device.
     * @param ctx An openCL context object.
     * @param fileName The name of the file containing the kernel source.
     * @return An OpenCL kernel object.
     */
    cl::Kernel kernel(cl::Context &ctx, std::string fileName) {
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

        // TODO(amidvidy): figure out how to reuse the device vector we created earlier?
        errNum = program.build(ctx.getInfo<CL_CONTEXT_DEVICES>());

        if (errNum != CL_SUCCESS) {
            std::cerr << "Failed to compile program." << std::endl;
            // TODO(amidvidy): show build log on failure

            //std::string buildLog;
            //program.getBuildInfo<std::string>((cl_program_build_info) CL_PROGRAM_BUILD_LOG, buildLog);
            //std::cerr << buildLog;
            exit(-1);
        }


        cl::Kernel kernel = cl::Kernel(program, "gaussian_filter", &errNum);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Failed to create kernel" << std::endl;
            exit(-1);
        }

        return kernel;

    }

} // namespace setup {

// Functions relating to image handling
namespace image {

    /**
     * Loads an image from a file into device texture memory.
     * @param ctx An openCL context object.
     * @param fileName The image file to use.
     * @param height An int ref to read the height into for later use.
     * @param width An int ref to read the width into for later use.
     * @return An openCL image2D object representing the image on the device.
     */
    cl::Image2D load(cl::Context &ctx, std::string fileName, int &height, int &width) {
        FREE_IMAGE_FORMAT format = FreeImage_GetFileType(fileName.c_str(), 0);
        FIBITMAP *image = FreeImage_Load(format, fileName.c_str());

        image = FreeImage_ConvertTo32Bits(image);
        width = FreeImage_GetWidth(image);
        height = FreeImage_GetHeight(image);

        char buffer[width * height * 4];

        memcpy(buffer, FreeImage_GetBits(image), width * height * 4);

        FreeImage_Unload(image);

        cl_int errNum;
        cl::ImageFormat imageFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);

        cl::Image2D img =  cl::Image2D(ctx,
                                       (cl_mem_flags) CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                       imageFormat,
                                       width,
                                       height,
                                       0,
                                       buffer,
                                       &errNum);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error loading image: " << fileName << std::endl;
            exit(-1);
        }

        return img;
    }

    /**
     * Creates a writable, empty image object in texture memory.
     * @param ctx An openCL context object.
     * @param height The desired height of the image object
     * @param width The desired width of the image object
     * @return An openCL Image2D object representing the texture memory.
     */
    cl::Image2D make(cl::Context &ctx, int height, int width) {
        cl_int errNum;

        cl::ImageFormat imageFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);

        cl::Image2D img = cl::Image2D(ctx,
                                      (cl_mem_flags) CL_MEM_READ_WRITE,
                                      imageFormat,
                                      width,
                                      height,
                                      0,
                                      NULL,
                                      &errNum);
        if (errNum != CL_SUCCESS) {
            std::cerr << "Error creating image." << std::endl;
            exit(-1);
        }
        return img;
    }

    /**
     * Creates an openCL sampler object for sampling continuous values from an image with
     * hardware acceleration.
     * @param ctx An openCL context object.
     * @return An openCL sampler object.
     */
    cl::Sampler sampler(cl::Context &ctx) {
        cl_int errNum;

        cl::Sampler sampler = cl::Sampler(ctx,
                                          CL_FALSE, // Non-normalized coordinates
                                          CL_ADDRESS_CLAMP_TO_EDGE,
                                          CL_FILTER_NEAREST,
                                          &errNum);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error creating CL sampler object." << std::endl;
            exit(-1);
        }

        return sampler;

    }

} // namespace image {

// Math utils
namespace math {

    /**
     * Utility function to round up to the nearest multiple of the group size.
     * @param groupSize The work group size.
     * @param globalSize The unrounded globalSize
     * @return The smallest multiple of groupSize greater than globalSize.
     */
    size_t roundUp(int groupSize, int globalSize) {
        int r = globalSize % groupSize;
        if (r == 0) {
            return globalSize;
        } else {
            return globalSize + groupSize - r;
        }
    }

} // namespace math

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "USAGE: seamcl <IMAGENAME>" << std::endl;
        exit(-1);
    }
    // Create OpenCL context
    cl::Context context = setup::context();
    // Create commandQueue
    cl::CommandQueue cmdQueue = setup::commandQueue(context);

    // Load image into a buffer
    int width, height;
    cl::Image2D inputImage = image::load(context, std::string(argv[1]), height, width);

    // Create output buffer
    cl::Image2D outputImage = image::make(context, height, width);

    // Make sampler
    cl::Sampler sampler = image::sampler(context);

    // Make kernel object
    cl::Kernel kernel = setup::kernel(context, std::string("ImageFilter2D.cl"));


    // Set kernel arguments
    cl_int errNum;

    errNum = kernel.setArg(0, inputImage);
    errNum |= kernel.setArg(1, outputImage);
    errNum |= kernel.setArg(2, sampler);
    errNum |= kernel.setArg(3, width);
    errNum |= kernel.setArg(4, height);

    if (errNum != CL_SUCCESS) {
        std::cerr << "Error setting kernel arguments." << std::endl;
        exit(-1);
    }

    // Determine local and global work size
    cl::NDRange localWorkSize = cl::NDRange(16, 16);
    cl::NDRange globalWorkSize = cl::NDRange(math::roundUp(localWorkSize[0], width),
                                             math::roundUp(localWorkSize[1], height));

    std::cout << "SUCCESS!" << std::endl;

}
