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
    if (argc < 5) {
        std::cerr << "USAGE: seamcl <INPUT> <OUTPUT> <DESIRED WIDTH> <DESIRED HEIGHT>" << std::endl;
        exit(-1);
    }

    // Parse arguments
    std::string inputFile(argv[1]);
    std::string outputFile(argv[2]);

    std::istringstream s1(argv[3]);
    std::istringstream s2(argv[4]);
    int desiredWidth, desiredHeight;
    if (! (s1 >> desiredWidth)) {
        std::cerr << "DESIRED WIDTH must be an integer." << std::endl;
        exit(-1);
    } else if (! (s2 >> desiredHeight)) {
        std::cerr << "DESIRED HEIGHT must be an integer." << std::endl;
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
    if (!image::save(cmdQueue, outputImage, outputFile, height, width)) {
        std::cerr << "Error writing output image: " << outputFile << std::endl;
        return -1;
    }

    std::cout << "SUCCESS!" << std::endl;

}
