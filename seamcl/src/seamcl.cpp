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

    // Intermediate buffer to hold blurred image. Currently unneeded
    //cl::Image2D blurredImage = image::make(context, height, width);

    // Allocate space on device for energy matrix
    cl::Buffer energyMatrix = mem::buffer(context, cmdQueue, height * width * sizeof(float));

    // Outer iterator
    //while (width > desiredWidth || height > desiredHeight) {


    // NOTE: Only one object detection kernel A-C can be left uncommented:

    // Kernel A: Blur image and then compute gradient.
    //kernel::blur(context, cmdQueue, inputImage, blurredImage, sampler, height, width);
    //kernel::gradient(context, cmdQueue, inputImage, blurredImage, sampler, height, width);

    // Kernel B: Convolve with Laplacian of Gaussian:
    kernel::laplacian(context, cmdQueue, inputImage, energyMatrix, sampler, height, width);

    // Kernel C: Convolve with Optimized Laplacian of Gaussian:


    // Perform dynamic programming top-bottom
    kernel::computeSeams(context, cmdQueue, energyMatrix, width, height, width);
    // TODO: transpose and perform dynamic programming left-right

    //}

    // Save image to disk.
    // TODO(amidvidy): this should be saving inputImage
    image::save(cmdQueue, inputImage, outputFile, height, width);

    std::cout << "SUCCESS!" << std::endl;

}
