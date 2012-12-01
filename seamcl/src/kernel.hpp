#ifndef KERNEL_HPP
#define KERNEL_HPP

// OpenCL
#include <CL/cl.hpp>

// STL
#include <iostream>

// SeamCL
#include "image.hpp"
#include "setup.hpp"
#include "math.hpp"
#include "mem.hpp"
#include "verify.hpp"

// Wrapper functions around calling kernels
namespace kernel {

    cl::Kernel blurKernel;
    cl::Kernel gradientKernel;
    cl::Kernel maskUnreachableKernel;
    cl::Kernel backtrackKernel;
    cl::Kernel findMinSeamVertKernel;
    cl::Kernel carveVertKernel;
    cl::Kernel computeSeamKernel;
    cl::Kernel DP_trapezoid;

    void init(cl::Context &ctx) {
        blurKernel = setup::kernel(ctx, std::string("GaussianKernel.cl"),
                                   std::string("gaussian_filter"));

        gradientKernel = setup::kernel(ctx, std::string("GradientKernel.cl"), std::string("image_gradient"));

        maskUnreachableKernel = setup::kernel(ctx, std::string("maskUnreachable.cl"),
                                              std::string("mask_unreachable"));

        backtrackKernel = setup::kernel(ctx, std::string("Backtrack.cl"),
                                        std::string("backtrack_vert"));

        computeSeamKernel = setup::kernel(ctx, std::string("computeSeams.cl"),
                                          std::string("computeSeams"));
	DP_trapezoidKernel = setup::kernel(ctx, string("DP_trapezoid.cl", std::string("DP_trapezoid"));

        findMinSeamVertKernel= setup::kernel(ctx, std::string("findMinVert.cl"),
                                             std::string("find_min_vert"));

        carveVertKernel = setup::kernel(ctx, std::string("CarveVert.cl"),
                                        std::string("carve_vert"));
    }

    /**
     * Applies a gaussian blur filter to an image using openCL.
     * @param ctx An openCL context object.
     * @param cmdQueue An openCL command queue.
     * @param sampler An openCL image sampler object.
     * @param height The height of the input image.
     * @param width The width of the input image.
     * @return An Image2D object containing the resulting image data.
     */
    void blur(cl::Context &ctx,
              cl::CommandQueue &cmdQueue,
              cl::Event &blurEvent,
              cl::Image2D &inputImage,
              cl::Image2D &outputImage,
              cl::Sampler &sampler,
              int height,
              int width,
              int colsRemoved) {

        // Set kernel arguments
        cl_int errNum;

        errNum = blurKernel.setArg(0, inputImage);
        errNum |= blurKernel.setArg(1, outputImage);
        errNum |= blurKernel.setArg(2, sampler);
        errNum |= blurKernel.setArg(3, width);
        errNum |= blurKernel.setArg(4, height);
        errNum |= blurKernel.setArg(5, colsRemoved);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error setting blurKernel arguments." << std::endl;
            std::cerr << errNum << std::endl;
            exit(-1);
        }

        // Determine local and global work size
        cl::NDRange offset = cl::NDRange(0, 0);
        cl::NDRange localWorkSize = cl::NDRange(16, 16);
        cl::NDRange globalWorkSize = cl::NDRange(math::roundUp(localWorkSize[0], width),
                                                 math::roundUp(localWorkSize[1], height));
        // Run blurKernel
        errNum = cmdQueue.enqueueNDRangeKernel(blurKernel,
                                               offset,
                                               globalWorkSize,
                                               localWorkSize,
                                               NULL,
                                               &blurEvent);
        if (errNum != CL_SUCCESS) {
            std::cerr << "Error enqueuing blur kernel for execution." << std::endl;
            exit(-1);
        }

    }

    /**
     * Computes the gradient of an image using openCL.
     * @param ctx An openCL context object.
     * @param cmdQueue An openCL command queue.
     * @param inputImage The image to blur
     * @param energyMatrix An openCL buffer to store the output in.
     * @param sampler An openCL image sampler object.
     * @param height The height of the input image.
     * @param width The width of the input image.
     * @return A buffer containing the gradient interpreted as a matrix of size height * width.
     */
    void gradient(cl::Context &ctx,
                  cl::CommandQueue &cmdQueue,
                  cl::Event &event,
                  std::vector<cl::Event> &deps,
                  cl::Image2D &inputImage,
                  cl::Buffer &energyMatrix,
                  cl::Sampler &sampler,
                  int height,
                  int width,
                  int colsRemoved) {
        cl_int errNum;

        // Set gradientKernel arguments
        errNum = gradientKernel.setArg(0, inputImage);
        errNum |= gradientKernel.setArg(1, energyMatrix);
        errNum |= gradientKernel.setArg(2, sampler);
        errNum |= gradientKernel.setArg(3, width);
        errNum |= gradientKernel.setArg(4, height);
        errNum |= gradientKernel.setArg(5, colsRemoved);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error setting gradient gradientKernel arguments." << std::endl;
            exit(-1);
        }

        cl::NDRange offset = cl::NDRange(0, 0);
        cl::NDRange localWorkSize = cl::NDRange(16, 16);
        cl::NDRange globalWorkSize = cl::NDRange(math::roundUp(localWorkSize[0], width),
                                                 math::roundUp(localWorkSize[1], height));

        errNum = cmdQueue.enqueueNDRangeKernel(gradientKernel,
                                               offset,
                                               globalWorkSize,
                                               localWorkSize,
                                               NULL,
                                               &event);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error enqueuing gradient kernel for execution." << std::endl;
            exit(-1);
        }

        // // TODO(amidvidy): make this debugging code
        // // Read data into an image object
        // cl::Image2D gradientImage = cl::Image2D(ctx,
        //                                         (cl_mem_flags) CL_MEM_READ_WRITE,
        //                                         cl::ImageFormat(CL_LUMINANCE, CL_FLOAT),
        //                                         width,
        //                                         height,
        //                                         0,
        //                                         NULL,
        //                                         &errNum);
        // if (errNum != CL_SUCCESS) {
        //     std::cerr << "Error creating gradient output image" << std::endl;
        //     exit(-1);
        // }

        // cl::size_t<3> origin;
        // origin.push_back(0);
        // origin.push_back(0);
        // origin.push_back(0);
        // cl::size_t<3> region;
        // region.push_back(width);
        // region.push_back(height);
        // region.push_back(1);

        // errNum = cmdQueue.enqueueCopyBufferToImage(energyMatrix,
        //                                            gradientImage,
        //                                            0,
        //                                            origin,
        //                                            region,
        //                                            NULL,
        //                                            NULL);

        // if (errNum != CL_SUCCESS) {
        //     std::cerr << "Error copying gradient image from buffer" << std::endl;
        //     std::cerr << "ERROR_CODE = " << errNum << std::endl;
        // }

        // image::save(cmdQueue, gradientImage, std::string("gradient_output.tif"), height, width);
        /** END DEBUGGING */

    }

    void maskUnreachable(cl::Context &ctx,
                         cl::CommandQueue &cmdQueue,
                         cl::Event &event,
                         std::vector<cl::Event> &deps,
                         cl::Buffer &energyMatrix,
                         int width,
                         int height,
                         int pitch,
                         int colsRemoved) {
        cl_int errNum;

        errNum = maskUnreachableKernel.setArg(0, energyMatrix);
        errNum |= maskUnreachableKernel.setArg(1, width);
        errNum |= maskUnreachableKernel.setArg(2, height);
        errNum |= maskUnreachableKernel.setArg(3, pitch);
        errNum |= maskUnreachableKernel.setArg(4, colsRemoved);


        if (errNum != CL_SUCCESS) {
            std::cerr << "Error setting maskUnreachable kernel arguments." << std::endl;
            exit(-1);
        }
        cl::NDRange offset = cl::NDRange(0, 0);
        cl::NDRange localWorkSize = cl::NDRange(16, 16);
        cl::NDRange globalWorkSize = cl::NDRange(math::roundUp(localWorkSize[0], width),
                                                 math::roundUp(localWorkSize[1], height));


        errNum = cmdQueue.enqueueNDRangeKernel(maskUnreachableKernel,
                                               offset,
                                               globalWorkSize,
                                               localWorkSize,
                                               &deps,
                                               &event);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error enqueueing maskUnreachable kernel." << std::endl;
            std::cerr << errNum << std::endl;
            exit(-1);
        }
    }

    void computeSeams(cl::Context &ctx,
                      cl::CommandQueue &cmdQueue,
                      cl::Event &event,
                      std::vector<cl::Event> &deps,
                      cl::Buffer &energyMatrix,
                      int width,
                      int height,
                      int pitch,
                      int colsRemoved) {

        cl_int errNum;

        // Set kernel arguments
        errNum = computeSeamKernel.setArg(0, energyMatrix);
        errNum |= computeSeamKernel.setArg(1, width);
        errNum |= computeSeamKernel.setArg(2, height);
        errNum |= computeSeamKernel.setArg(3, pitch);
        errNum |= computeSeamKernel.setArg(4, colsRemoved);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error setting computeSeam kernel arguments." << std::endl;
            exit(-1);
        }

        cl::NDRange offset = cl::NDRange(0);
        cl::NDRange localWorkSize = cl::NDRange(256);
        cl::NDRange globalWorkSize = cl::NDRange(256);

        //TODO(amidvidy): this should be configurable with a flag

        // /** DEBUGGING */
        // float *originalEnergyMatrix = new float[width * height];
        // // read in original data

        // mem::read(ctx, cmdQueue, originalEnergyMatrix, energyMatrix, width * height);

        /** END DEBUGGING **/

        errNum = cmdQueue.enqueueNDRangeKernel(computeSeamKernel,
                                               offset,
                                               globalWorkSize,
                                               localWorkSize,
                                               &deps,
                                               &event);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error enqueuing computeSeams kernel for execution." << std::endl;
            // delete [] originalEnergyMatrix;
            exit(-1);
        }

        // /** DEBUGGING **/
        // float *deviceResult = new float[width * height];
        // mem::read(ctx, cmdQueue, deviceResult, energyMatrix, width * height);

        // if(!verify::computeSeams(deviceResult, originalEnergyMatrix, width, height, pitch, colsRemoved)) {
        //     std::cerr << "Incorrect results from kernel::computeSeams" << std::endl;
        //     delete [] originalEnergyMatrix;
        //     delete [] deviceResult;
        //     exit(-1);
        // }

        // delete [] originalEnergyMatrix;
        // delete [] deviceResult;
        // /** END DEBUGGING **/
    }

    void backtrack(cl::Context &ctx,
                   cl::CommandQueue &cmdQueue,
                   cl::Event &event,
                   std::vector<cl::Event> &deps,
                   cl::Buffer &energyMatrix,
                   cl::Buffer &vertSeamPath,
                   cl::Buffer &vertMinIdx,
                   int width,
                   int height,
                   int pitch,
                   int colsRemoved) {

        cl_int errNum;

        // Set kernel arguments
        errNum = backtrackKernel.setArg(0, energyMatrix);
        errNum |= backtrackKernel.setArg(1, vertSeamPath);
        errNum |= backtrackKernel.setArg(2, vertMinIdx);
        errNum |= backtrackKernel.setArg(3, width);
        errNum |= backtrackKernel.setArg(4, height);
        errNum |= backtrackKernel.setArg(5, pitch);
        errNum |= backtrackKernel.setArg(6, colsRemoved);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error setting backtrack kernel arguments." << std::endl;
            exit(-1);
        }

        cl::NDRange offset = cl::NDRange(0);
        cl::NDRange localWorkSize = cl::NDRange(1);
        cl::NDRange globalWorkSize = cl::NDRange(256);

        errNum = cmdQueue.enqueueNDRangeKernel(backtrackKernel,
                                               offset,
                                               globalWorkSize,
                                               localWorkSize,
                                               &deps,
                                               &event);


        if (errNum != CL_SUCCESS) {
            std::cerr << "Error enqueueing backTrack kernel for execution." << std::endl;
            exit(-1);
        }

        // /** DEBUGGING **/
        // int deviceResult[height];

        // mem::read(ctx, cmdQueue, deviceResult, vertSeamPath, height);
        // for (int i = height - 5; i < height; ++i) {
        //     std::cout << "deviceResult[" << i << "]=\t" << deviceResult[i] << std::endl;
        // }

    }

    void findMinSeamVert(cl::Context &ctx,
                         cl::CommandQueue &cmdQueue,
                         cl::Event &event,
                         std::vector<cl::Event> &deps,
                         cl::Buffer &energyMatrix,
                         cl::Buffer &vertMinEnergy,
                         cl::Buffer &vertMinIdx,
                         int width,
                         int height,
                         int pitch,
                         int colsRemoved) {

        cl_int errNum;
        errNum = findMinSeamVertKernel.setArg(0, energyMatrix);
        errNum |= findMinSeamVertKernel.setArg(1, vertMinEnergy);
        errNum |= findMinSeamVertKernel.setArg(2, vertMinIdx);
        errNum |= findMinSeamVertKernel.setArg(3, cl::__local(256 * sizeof(float)));
        errNum |= findMinSeamVertKernel.setArg(4, cl::__local(256 * sizeof(float)));
        errNum |= findMinSeamVertKernel.setArg(5, width);
        errNum |= findMinSeamVertKernel.setArg(6, height);
        errNum |= findMinSeamVertKernel.setArg(7, pitch);
        errNum |= findMinSeamVertKernel.setArg(8, colsRemoved);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error setting findMinSeamVert arguments." << std::endl;
            exit(-1);
        }

        // This kernel could be written to use more than one work group, but its probably not worth it.

        cl::NDRange offset = cl::NDRange(0);
        cl::NDRange localWorkSize = cl::NDRange(256);
        cl::NDRange globalWorkSize = cl::NDRange(256);

        errNum = cmdQueue.enqueueNDRangeKernel(findMinSeamVertKernel,
                                               offset,
                                               globalWorkSize,
                                               localWorkSize,
                                               &deps,
                                               &event);
        if (errNum != CL_SUCCESS) {
            std::cerr << "Error enqueuing computeSeams kernel for execution." << std::endl;
            exit(-1);
        }

        /** DEBUG **/
        // int deviceResultIdx[1];
        // float deviceResultEnergy[1];

        // mem::read(ctx, cmdQueue, deviceResultIdx, vertMinIdx);
        // mem::read(ctx, cmdQueue, deviceResultEnergy, vertMinEnergy);

        // std::cout << "deviceResultIdx = " << deviceResultIdx[0] << std::endl;
        // std::cout << "deviceResultEnergy = " << deviceResultEnergy[0] << std::endl;
    }

    /**
     * Computes the laplacian of the gaussian convolution
     *  with an image (using openCL).
     * @param ctx An openCL context object.
     * @param cmdQueue An openCL command queue.
     * @param inputImage The image to blur
     * @param energyMatrix An openCL buffer to store the output in.
     * @param sampler An openCL image sampler object.
     * @param height The height of the input image.
     * @param width The width of the input image.
     * @return A buffer containing the gradient interpreted as a matrix of size height * width.
     */
    void laplacian(cl::Context &ctx,
                  cl::CommandQueue &cmdQueue,
                  cl::Image2D &inputImage,
                  cl::Buffer &energyMatrix,
                  cl::Sampler &sampler,
                  int height,
                  int width) {


        // Setup kernel
        cl::Kernel kernel = setup::kernel(ctx, std::string("LaplacianGaussianKernel.cl"),
                                          std::string("gaussian_laplacian"));

        cl_int errNum;

        // Set kernel arguments
        errNum = kernel.setArg(0, inputImage);
        errNum |= kernel.setArg(1, energyMatrix);
        errNum |= kernel.setArg(2, sampler);
        errNum |= kernel.setArg(3, width);
        errNum |= kernel.setArg(4, height);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error setting laplacian kernel arguments." << std::endl;
            exit(-1);
        }

        cl::NDRange offset = cl::NDRange(0, 0);
        cl::NDRange localWorkSize = cl::NDRange(16, 16);
        cl::NDRange globalWorkSize = cl::NDRange(math::roundUp(localWorkSize[0], width),
                                                 math::roundUp(localWorkSize[1], height));

        errNum = cmdQueue.enqueueNDRangeKernel(kernel,
                                               offset,
                                               globalWorkSize,
                                               localWorkSize);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error enqueuing laplacian kernel for execution." << std::endl;
            exit(-1);
        }

        // TODO(amidvidy): make this debugging code
        // Read data into an image object
        cl::Image2D gradientImage = cl::Image2D(ctx,
                                                (cl_mem_flags) CL_MEM_READ_WRITE,
                                                cl::ImageFormat(CL_INTENSITY, CL_FLOAT),
                                                width,
                                                height,
                                                0,
                                                NULL,
                                                &errNum);
        if (errNum != CL_SUCCESS) {
            std::cerr << "Error creating laplacian output image" << std::endl;
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

        errNum = cmdQueue.enqueueCopyBufferToImage(energyMatrix,
                                                   gradientImage,
                                                   0,
                                                   origin,
                                                   region,
                                                   NULL,
                                                   NULL);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error copying laplacian image from buffer" << std::endl;
            std::cerr << "ERROR_CODE = " << errNum << std::endl;
        }

        image::save(cmdQueue, gradientImage, std::string("gradient_output.tif"), height, width);
        /** END DEBUGGING */

    } // End of laplacian method.


    void paintSeam(cl::Context &ctx,
                   cl::CommandQueue &cmdQueue,
                   cl::Image2D &inputImage,
                   cl::Buffer &vertSeamPath,
                   int width,
                   int height) {

        cl::Kernel kernel = setup::kernel(ctx, std::string("PaintSeam.cl"), std::string("paint_seam"));

        cl_int errNum;

        errNum = kernel.setArg(0, inputImage);
        errNum |= kernel.setArg(1, vertSeamPath);
        errNum |= kernel.setArg(2, width);
        errNum |= kernel.setArg(3, height);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error setting paintseam kernel arguments." << std::endl;
            exit(-1);
        }

        cl::NDRange offset = cl::NDRange(0, 0);
        cl::NDRange localWorkSize = cl::NDRange(16, 16);
        cl::NDRange globalWorkSize = cl::NDRange(math::roundUp(localWorkSize[0], width),
                                                 math::roundUp(localWorkSize[1], height));
        std::cout << "launching PaintSeam" << std::endl;
        errNum = cmdQueue.enqueueNDRangeKernel(kernel,
                                               offset,
                                               globalWorkSize,
                                               localWorkSize);
        if (errNum != CL_SUCCESS) {
            std::cerr << "Error enqueuing paintSeam kernel for execution." << std::endl;
            exit(-1);
        }

    }

    void carveVert(cl::Context &ctx,
                   cl::CommandQueue &cmdQueue,
                   cl::Event &event,
                   std::vector<cl::Event> &deps,
                   cl::Image2D &inputImage,
                   cl::Image2D &outputImage,
                   cl::Buffer &vertSeamPath,
                   cl::Sampler &sampler,
                   int width,
                   int height,
                   int numRowsCarved) {

        cl_int errNum;

        errNum = carveVertKernel.setArg(0, inputImage);
        errNum |= carveVertKernel.setArg(1, outputImage);
        errNum |= carveVertKernel.setArg(2, vertSeamPath);
        errNum |= carveVertKernel.setArg(3, sampler);
        errNum |= carveVertKernel.setArg(4, width);
        errNum |= carveVertKernel.setArg(5, height);
        errNum |= carveVertKernel.setArg(6, numRowsCarved);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error setting carveVert kernel arguments." << std::endl;
            exit(-1);
        }

        cl::NDRange offset = cl::NDRange(0, 0);
        cl::NDRange localWorkSize = cl::NDRange(16, 16);
        cl::NDRange globalWorkSize = cl::NDRange(math::roundUp(localWorkSize[0], width),
                                                 math::roundUp(localWorkSize[1], height));
        errNum = cmdQueue.enqueueNDRangeKernel(carveVertKernel,
                                               offset,
                                               globalWorkSize,
                                               localWorkSize,
                                               &deps,
                                               &event);
        if (errNum != CL_SUCCESS) {
            std::cerr << "Error enqueueing carveVert kernel for execution." << std::endl;
            exit(-1);
        }
    }

} // namespace kernel {

#endif
