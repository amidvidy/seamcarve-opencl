// C
#include <cstdlib>

// STL
#include <algorithm>
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
    int colsToRemove;
    setup::args(argc, argv, inputFile, outputFile, colsToRemove);

    // Create OpenCL context
    cl::Context context = setup::context();
    // Create commandQueue
    cl::CommandQueue cmdQueue = setup::commandQueue(context);

    // Load image into a buffer
    int width, height;
    cl::Image2D inputImage = image::load(context, inputFile, height, width);
    char *origCharBuffer = 0;
    cl::Buffer inputImageBuffer = image::loadBuffer(context, cmdQueue, inputFile, height, width, origCharBuffer);

    // just in case we end up using padding
    int pitch = width;

    // Make sampler
    //cl::Sampler sampler = image::sampler(context);

    // Intermediate buffer to hold blurred image.
    //cl::Image2D blurredImage = image::make(context, height, width);
    cl::Buffer blurredImageBuffer = mem::buffer(context, cmdQueue, height * width * 4);

    // Allocate space on device for energy matrix
    cl::Buffer energyMatrix = mem::buffer(context, cmdQueue, height * width * sizeof(float));

    // Holds the current energy of the min vertical seam
    cl::Buffer vertMinEnergy = mem::buffer(context, cmdQueue, sizeof(float));
    // Holds the starting index of the min vertical seam
    cl::Buffer vertMinIdx = mem::buffer(context, cmdQueue, sizeof(int));
    // Holds the indexes of of the min vertical seam
    cl::Buffer vertSeamPath = mem::buffer(context, cmdQueue, sizeof(int) * height);

    // Init kernels
    kernel::init(context);

    // We are going to need to swap pointers each iteration
    //cl::Image2D *curInputImage = &inputImage;
    //cl::Image2D *curOutputImage = &blurredImage;

    cl::Buffer *curInputImage = &inputImageBuffer;
    cl::Buffer *curOutputImage = &blurredImageBuffer;

    uint64 totalTimeMillis = 0;

    int colsRemoved = 0;

    // Events
    cl::Event blurEvent;
    cl::Event gradientEvent;
    cl::Event maskUnreachableEvent;
    cl::Event computeSeamsEvent;
    cl::Event findMinSeamVertEvent;
    cl::Event backtrackEvent;
    cl::Event carveVertEvent;

    // Dependencies, unused for now.
    std::vector<cl::Event> gradientDeps;
    std::vector<cl::Event> maskUnreachableDeps;
    std::vector<cl::Event> computeSeamsDeps;
    std::vector<cl::Event> findMinSeamVertDeps;
    std::vector<cl::Event> backtrackDeps;
    std::vector<cl::Event> carveVertDeps;

    // Profiling
    cl_ulong blurTimeMicros = 0;
    cl_ulong gradientTimeMicros = 0;
    cl_ulong maskUnreachableTimeMicros = 0;
    cl_ulong computeSeamsTimeMicros = 0;
    cl_ulong findMinSeamVertTimeMicros = 0;
    cl_ulong backtrackTimeMicros = 0;
    cl_ulong carveVertTimeMicros = 0;

    cl_ulong kernelStartTime, kernelEndTime;

    // Outer iterator, still need to figure out height
    //while (width > desiredWidth || height > desiredHeight) {
    while (colsRemoved < colsToRemove) {
        std::cout << "Starting iteration:\t#" << colsRemoved << std::endl;
        uint64 startTime = verify::timeMillis();

        // NOTE: Only one object detection kernel A-C can be left uncommented:

        // Kernel A: Blur image and then compute gradient.
        kernel::blur(context, cmdQueue, blurEvent,
                     *curInputImage, *curOutputImage,
                     height, width, colsRemoved);

        kernel::gradient(context, cmdQueue,
                         gradientEvent, gradientDeps,
                         *curOutputImage,
                         energyMatrix,
                         height, width, colsRemoved);

        // Kernel B: Convolve with Laplacian of Gaussian:
        //kernel::laplacian(context, cmdQueue, *curInputImage, energyMatrix, sampler, height, width, colsRemoved);

        // Kernel C: Convolve with Optimized Laplacian of Gaussian:


        // Mask garbage values from previous iterations as well as stencil artifacts
        kernel::maskUnreachable(context, cmdQueue,
                                maskUnreachableEvent, maskUnreachableDeps,
                                energyMatrix,
                                width, height, pitch, colsRemoved);

        // Perform dynamic programming top-bottom
         kernel::computeSeams(context, cmdQueue,
                             computeSeamsEvent, computeSeamsDeps,
                             energyMatrix,
                             width, height, pitch, colsRemoved);

         // Kernel D: Do dynammic programming with Trapezoid (height = 4):
         //kernel::DP_trapezoidKernel(context, cmdQueue, computeSeamsEvent, computeSeamsDeps, energyMatrix, width, height, pitch, colsRemoved, 4);
        // Find min vertical seam
        kernel::findMinSeamVert(context, cmdQueue,
                                findMinSeamVertEvent, findMinSeamVertDeps,
                                energyMatrix, vertMinEnergy, vertMinIdx,
                                width, height, pitch, colsRemoved);

        // Backtrack
        kernel::backtrack(context, cmdQueue,
                          backtrackEvent, backtrackDeps,
                          energyMatrix, vertSeamPath, vertMinIdx,
                          width, height, pitch, colsRemoved);

        // for debugging
        //kernel::paintSeam(context, cmdQueue, inputImage, vertSeamPath, width, height);

        kernel::carveVert(context, cmdQueue,
                          carveVertEvent, carveVertDeps,
                          *curInputImage, *curOutputImage,
                          vertSeamPath,
                          width, height, colsRemoved + 1);

        cmdQueue.flush(); // Is this call needed?
        cmdQueue.finish();
        ++colsRemoved;

        // Swap pointers
        std::swap(curInputImage, curOutputImage);

        totalTimeMillis += (verify::timeMillis() - startTime);

        blurEvent.getProfilingInfo(CL_PROFILING_COMMAND_START, &kernelStartTime);
        blurEvent.getProfilingInfo(CL_PROFILING_COMMAND_END, &kernelEndTime);
        blurTimeMicros += (kernelEndTime - kernelStartTime);

        gradientEvent.getProfilingInfo(CL_PROFILING_COMMAND_START, &kernelStartTime);
        gradientEvent.getProfilingInfo(CL_PROFILING_COMMAND_END, &kernelEndTime);
        gradientTimeMicros += (kernelEndTime - kernelStartTime);

        maskUnreachableEvent.getProfilingInfo(CL_PROFILING_COMMAND_START, &kernelStartTime);
        maskUnreachableEvent.getProfilingInfo(CL_PROFILING_COMMAND_END, &kernelEndTime);
        maskUnreachableTimeMicros += (kernelEndTime - kernelStartTime);

        computeSeamsEvent.getProfilingInfo(CL_PROFILING_COMMAND_START, &kernelStartTime);
        computeSeamsEvent.getProfilingInfo(CL_PROFILING_COMMAND_END, &kernelEndTime);
        computeSeamsTimeMicros += (kernelEndTime - kernelStartTime);

        findMinSeamVertEvent.getProfilingInfo(CL_PROFILING_COMMAND_START, &kernelStartTime);
        findMinSeamVertEvent.getProfilingInfo(CL_PROFILING_COMMAND_END, &kernelEndTime);
        findMinSeamVertTimeMicros += (kernelEndTime - kernelStartTime);

        backtrackEvent.getProfilingInfo(CL_PROFILING_COMMAND_START, &kernelStartTime);
        backtrackEvent.getProfilingInfo(CL_PROFILING_COMMAND_END, &kernelEndTime);
        backtrackTimeMicros += (kernelEndTime - kernelStartTime);

        carveVertEvent.getProfilingInfo(CL_PROFILING_COMMAND_START, &kernelStartTime);
        carveVertEvent.getProfilingInfo(CL_PROFILING_COMMAND_END, &kernelEndTime);
        carveVertTimeMicros += (kernelEndTime - kernelStartTime);
    }

    // Save image to disk.
    // TODO(amidvidy): this should be saving inputImage
    //image::save(cmdQueue, *curOutputImage, outputFile, height, width);
    char *resultCharBuffer = 0;
    image::saveBuffer(context, cmdQueue, *curOutputImage, outputFile, height, width, resultCharBuffer);


    // TODO(amidvidy): this debugging code is no longer needed.
    std::cout << std::endl;
    std::cout << "Checking buffer representation of image..." << std::endl;
    if (!verify::arraysEqual(origCharBuffer, resultCharBuffer, height * width * 4)) {
        std::cout << "Arrays do not match!!!" << std::endl;
    }
    delete [] origCharBuffer;
    delete [] resultCharBuffer;

    std::cout << std::endl;
    std::cout << "Carve completed!" << std::endl;
    std::cout << std::endl;
    std::cout << "Avg total time per iteration:\t" << totalTimeMillis / colsToRemove << " millis" << std::endl;
    std::cout << "Avg time for blur:\t" << blurTimeMicros / colsToRemove << " nanos" << std::endl;
    std::cout << "Avg time for gradient:\t" << gradientTimeMicros / colsToRemove << " nanos" << std::endl;
    std::cout << "Avg time for maskUnreachable:\t" << maskUnreachableTimeMicros / colsToRemove << " nanos" << std::endl;
    std::cout << "Avg time for computeSeams\t" << computeSeamsTimeMicros / colsToRemove << " nanos" << std::endl;
    std::cout << "Avg time for findMinSeamVert\t" << findMinSeamVertTimeMicros / colsToRemove << " nanos" << std::endl;
    std::cout << "Avg time for backtrack\t" << backtrackTimeMicros / colsToRemove << " nanos" << std::endl;
    std::cout << "Avg time for carveVert\t" << carveVertTimeMicros / colsToRemove << " nanos" << std::endl;
}
