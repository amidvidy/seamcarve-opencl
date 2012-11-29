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

    // just in case we end up using padding
    int pitch = width;

    // Make sampler
    cl::Sampler sampler = image::sampler(context);

    // Intermediate buffer to hold blurred image.
    cl::Image2D blurredImage = image::make(context, height, width);

    // Allocate space on device for energy matrix
    cl::Buffer energyMatrix = mem::buffer(context, cmdQueue, height * width * sizeof(float));

    // Holds the current energy of the min vertical seam
    cl::Buffer vertMinEnergy = mem::buffer(context, cmdQueue, sizeof(float));
    // Holds the starting index of the min vertical seam
    cl::Buffer vertMinIdx = mem::buffer(context, cmdQueue, sizeof(int));
    // Holds the indexes of of the min vertical seam
    cl::Buffer vertSeamPath = mem::buffer(context, cmdQueue, sizeof(int) * height);

    // Outer iterator
    //while (width > desiredWidth || height > desiredHeight) {


    // NOTE: Only one object detection kernel A-C can be left uncommented:

    // Kernel A: Blur image and then compute gradient.
    kernel::blur(context, cmdQueue, inputImage, blurredImage, sampler, height, width);
    kernel::gradient(context, cmdQueue, blurredImage, energyMatrix, sampler, height, width);

    // Kernel B: Convolve with Laplacian of Gaussian:
    //kernel::laplacian(context, cmdQueue, inputImage, energyMatrix, sampler, height, width);

    // Kernel C: Convolve with Optimized Laplacian of Gaussian:


    // Perform dynamic programming top-bottom
    kernel::computeSeams(context, cmdQueue, energyMatrix, width, height, pitch);
    // TODO: transpose and perform dynamic programming left-right

    // Find min vertical seam
    kernel::findMinSeamVert(context, cmdQueue, energyMatrix, vertMinEnergy, vertMinIdx, width, height, pitch);


    // Backtrack
    kernel::backtrack(context, cmdQueue, energyMatrix, vertSeamPath, vertMinIdx, width, height, pitch);
    cmdQueue.flush();
    // for debugging
    //kernel::paintSeam(context, cmdQueue, inputImage, vertSeamPath, width, height);

    kernel::carveVert(context, cmdQueue, inputImage, blurredImage, vertSeamPath, sampler, width, height, 1);
    //}

    // Save image to disk.
    // TODO(amidvidy): this should be saving inputImage
    cmdQueue.flush();

    image::save(cmdQueue, blurredImage, outputFile, height, width);

    std::cout << "SUCCESS!" << std::endl;

}
