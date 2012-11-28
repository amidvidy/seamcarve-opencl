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
#include "verify.hpp"


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

    // Allocate temp space for blurred image
    cl::Image2D blurredImage = image::make(context, height, width);

    // Allocate space on device for energy matrix
    cl::Buffer energyMatrix = mem::buffer(context, cmdQueue, height * width * sizeof(float));

    // Outer iterator
    //while (width > desiredWidth || height > desiredHeight) {

    // Blur image
    kernel::blur(context, cmdQueue, inputImage, blurredImage, sampler, height, width);
    // Calculate gradient (TODO!)
    kernel::laplacian(context, cmdQueue, blurredImage, energyMatrix, sampler, height, width);
    // Perform dynamic programming top-bottom
    kernel::computeSeams(context, cmdQueue, energyMatrix, 1, width, height, width);
    // TODO: transpose and perform dynamic programming left-right

    //}

    // Save image to disk.
    // TODO(amidvidy): this should be saving inputImage
    image::save(cmdQueue, blurredImage, outputFile, height, width);

    std::cout << "SUCCESS!" << std::endl;

}
