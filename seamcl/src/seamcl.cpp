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

// SeamCL
#include "image.hpp"
#include "kernel.hpp"
#include "math.hpp"
#include "mem.hpp"
#include "setup.hpp"


int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "USAGE: seamcl <INPUT> <OUTPUT>" << std::endl;
        exit(-1);
    }
    // Create OpenCL context
    cl::Context context = setup::context();
    // Create commandQueue
    cl::CommandQueue cmdQueue = setup::commandQueue(context);

    // Load image into a buffer
    int width, height;
    cl::Image2D inputImage = image::load(context, std::string(argv[1]), height, width);

    // Make sampler
    cl::Sampler sampler = image::sampler(context);

    // Blur image
    cl::Image2D outputImage = kernel::blur(context, cmdQueue, inputImage, sampler, height, width);

    std::cout << "before calc gradient" << std::endl;

    // Calculate gradient
    cl::Buffer gradient = kernel::gradient(context, cmdQueue, inputImage, sampler, height, width);

    /** START DEBUGGING */
    cl_int errNum;
    // Read data into an image object
    cl::Image2D gradientImage = cl::Image2D(context,
                                            (cl_mem_flags) CL_MEM_READ_WRITE,
                                            cl::ImageFormat(CL_LUMINANCE, CL_FLOAT),
                                            width,
                                            height,
                                            0,
                                            NULL,
                                            &errNum);
    if (errNum != CL_SUCCESS) {
        std::cerr << "Error creating gradient output image" << std::endl;
        exit(-1);
    }

    cl::size_t<3> origin;
    origin.push_back(0);
    origin.push_back(0);
    origin.push_back(0);
    cl::size_t<3> region;
    region.push_back(width);
    region.push_back(height);
    region.push_back(1);

    std::cerr << "Before enqueueCopyBufferToImage" << std::endl;

    errNum = cmdQueue.enqueueCopyBufferToImage(gradient,
                                               gradientImage,
                                               0,
                                               origin,
                                               region,
                                               NULL,
                                               NULL);
    std::cerr << "After enqueueCopyBufferToImage" << std::endl;

    if (errNum != CL_SUCCESS) {
        std::cerr << "Error copying gradient image from buffer" << std::endl;
        std::cerr << "ERROR_CODE = " << errNum << std::endl;
    }

    if (!image::save(cmdQueue, gradientImage, std::string("gradient_output.tif"), height, width)) {
        std::cerr << "Error writing gradient output image." << std::endl;
    }
    /** END DEBUGGING */

    // Save image to disk.
    std::string outputFile(argv[2]);

    if (!image::save(cmdQueue, outputImage, outputFile, height, width)) {
        std::cerr << "Error writing output image: " << outputFile << std::endl;
        return -1;
    }

    std::cout << "SUCCESS!" << std::endl;

}
