// C
#include <cstdlib>

// STL
#include <iostream>

// OpenCL
#include <CL/cl.hpp>

// FreeImage
#include "FreeImage.h"

namespace setup {

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

} // namespace setup {

namespace image {
    char * load(std::string fileName, int &height, int &width) {
        FREE_IMAGE_FORMAT format = FreeImage_GetFileType(fileName.c_str(), 0);
        FIBITMAP *image = FreeImage_Load(format, fileName.c_str());

        image = FreeImage_ConvertTo32Bits(image);
        width = FreeImage_GetWidth(image);
        height = FreeImage_GetHeight(image);

        char *buffer = new char[width * height * 4];
        memcpy(buffer, FreeImage_GetBits(image), width * height * 4);

        FreeImage_Unload(image);

        return buffer;
    }
} // namespace image {

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
    char *pixels = image::load(std::string(argv[1]), height, width);

    delete [] pixels;
}
