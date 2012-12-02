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

    char *imgBytes = 0;

    // Load image into a buffer
    int width, height;
    //cl::Image2D inputImage = image::load(context, inputFile, height, width);
    //cl::Buffer inputImage = image::loadBuffer(context, cmdQueue, inputFile, height, width);
    cl::Buffer inputImage = image::loadBuffer(context, cmdQueue, inputFile, height, width, imgBytes);

    if (colsToRemove > width) {
        std::cerr << "Trying to remove more columns then there are in the picture!" << std::endl;
        exit(-1);
    }

    // just in case we end up using padding
    int pitch = width;

    // Make sampler
    //cl::Sampler sampler = image::sampler(context);

    // Intermediate buffer to hold blurred image.
    cl::Buffer blurredImage = mem::buffer(context, cmdQueue, height * width * 4); // 32 bit pixels

    //cl::Image2D blurredImage = image::make(context, height, width);

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
    cl::Buffer *curInputImage = &inputImage;
    cl::Buffer *curOutputImage = &blurredImage;
    //cl::Image2D *curInputImage = &inputImage;
    //cl::Image2D *curOutputImage = &blurredImage;

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

    // Testing
    if(!verify::testMemReadWrite(context, cmdQueue, height * width)) {
        exit(-1);
    }


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
        // TODO: transpose and perform dynamic programming left-right

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

    char *imgOutBytes = 0;



    cmdQueue.finish();
    // Save image to disk.
    // TODO(amidvidy): this should be saving inputImage
    image::saveBuffer(context, cmdQueue, *curOutputImage, outputFile, height, width, imgOutBytes);

    int diffs = 0;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            if (imgOutBytes[i*4 + j] != imgBytes[i + j*4]) {
                diffs++;
            }
        }
    }
    if (diffs > 0) std::cerr << "Found " << diffs << " mismatches!" << std::endl;

    std::cout << std::endl;
    std::cout << "Carve completed!" << std::endl;
    std::cout << std::endl;
    std::cout << "Avg total time per iteration:\t" << totalTimeMillis / colsToRemove << " millis" << std::endl;
    std::cout << "Avg time for blur:\t" << blurTimeMicros / colsToRemove << " micros" << std::endl;
    std::cout << "Avg time for gradient:\t" << gradientTimeMicros / colsToRemove << " micros" << std::endl;
    std::cout << "Avg time for maskUnreachable:\t" << maskUnreachableTimeMicros / colsToRemove << " micros" << std::endl;
    std::cout << "Avg time for computeSeams\t" << computeSeamsTimeMicros / colsToRemove << " micros" << std::endl;
    std::cout << "Avg time for findMinSeamVert\t" << findMinSeamVertTimeMicros / colsToRemove << " micros" << std::endl;
    std::cout << "Avg time for backtrack\t" << backtrackTimeMicros / colsToRemove << " micros" << std::endl;
    std::cout << "Avg time for carveVert\t" << carveVertTimeMicros / colsToRemove << " micros" << std::endl;
}
