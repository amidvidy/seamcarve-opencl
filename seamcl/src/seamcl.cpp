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

    // Calculate gradient
    cl::Buffer gradient = kernel::gradient(context, cmdQueue, inputImage, sampler, height, width);

    // Save image to disk.
    std::string outputFile(argv[2]);

    if (!image::save(cmdQueue, outputImage, outputFile, height, width)) {
        std::cerr << "Error writing output image: " << outputFile << std::endl;
        return -1;
    }

    std::cout << "SUCCESS!" << std::endl;

}
