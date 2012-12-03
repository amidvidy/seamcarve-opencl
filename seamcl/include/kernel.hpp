#ifndef KERNEL_HPP
#define KERNEL_HPP

// OpenCL
#include "ocl.hpp"

// STL
#include <iostream>


// Wrapper functions around calling kernels
namespace kernel {

    void init(cl::Context &ctx);

    /**
     * Applies a gaussian blur filter to an image using openCL
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
              cl::Buffer &inputImage,
              cl::Buffer &outputImage,
              int height,
              int width,
              int colsRemoved);

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
                  cl::vector<cl::Event> &deps,
                  cl::Buffer &inputImage,
                  cl::Buffer &energyMatrix,
                  int height,
                  int width,
                  int colsRemoved);

    void maskUnreachable(cl::Context &ctx,
                         cl::CommandQueue &cmdQueue,
                         cl::Event &event,
                         cl::vector<cl::Event> &deps,
                         cl::Buffer &energyMatrix,
                         int width,
                         int height,
                         int pitch,
                         int colsRemoved);

    void computeSeams(cl::Context &ctx,
                      cl::CommandQueue &cmdQueue,
                      cl::Event &event,
                      cl::vector<cl::Event> &deps,
                      cl::Buffer &energyMatrix,
                      int width,
                      int height,
                      int pitch,
                      int colsRemoved);

    void backtrack(cl::Context &ctx,
                   cl::CommandQueue &cmdQueue,
                   cl::Event &event,
                   cl::vector<cl::Event> &deps,
                   cl::Buffer &energyMatrix,
                   cl::Buffer &vertSeamPath,
                   cl::Buffer &vertMinIdx,
                   int width,
                   int height,
                   int pitch,
                   int colsRemoved);

    void findMinSeamVert(cl::Context &ctx,
                         cl::CommandQueue &cmdQueue,
                         cl::Event &event,
                         cl::vector<cl::Event> &deps,
                         cl::Buffer &energyMatrix,
                         cl::Buffer &vertMinEnergy,
                         cl::Buffer &vertMinIdx,
                         int width,
                         int height,
                         int pitch,
                         int colsRemoved);

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
                  int width);


    void paintSeam(cl::Context &ctx,
                   cl::CommandQueue &cmdQueue,
                   cl::Image2D &inputImage,
                   cl::Buffer &vertSeamPath,
                   int width,
                   int height);

    void carveVert(cl::Context &ctx,
                   cl::CommandQueue &cmdQueue,
                   cl::Event &event,
                   cl::vector<cl::Event> &deps,
                   cl::Buffer &inputImage,
                   cl::Buffer &outputImage,
                   cl::Buffer &vertSeamPath,
                   int width,
                   int height,
                   int numRowsCarved);

} // namespace kernel

#endif
