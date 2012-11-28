// C
#include <cstdlib>

// STL
#include <fstream>
#include <sstream>
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

    // Parse arguments
    std::string inputFile, outputFile;
    int desiredWidth, desiredHeight;
    setup::args(argc, argv, inputFile, outputFile, desiredWidth, desiredHeight);

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
    image::save(cmdQueue, outputImage, outputFile, height, width);

    std::cout << "SUCCESS!" << std::endl;

}
