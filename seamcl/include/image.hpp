#ifndef IMAGE_HPP
#define IMAGE_HPP

// OpenCL
#include "ocl.hpp"

// Functions relating to image handling
namespace image {


    /**
     * Loads an image from a file into device texture memory.
n     * @param ctx An openCL context object.
     * @param fileName The image file to use.
     * @param height An int ref to read the height into for later use.
     * @param width An int ref to read the width into for later use.
     * @return An openCL image2D object representing the image on the device.
     */
    cl::Image2D load(cl::Context &ctx, std::string fileName, int &height, int &width);
    cl::Buffer loadBuffer(cl::Context &ctx,
                          cl::CommandQueue &cmdQueue,
                          std::string fileName,
                          int &height,
                          int &width,
                          char *& img);

    /**
     * Saves the contents of an image object to disk.
     * @param cmdQueue An openCL commandQueue object.
     * @param image The image object to read data from.
     * @param fileName The file in which to store the resulting image.
     * @param height The height of the image.
     * @param width The width of the image.
     * @return Whether the operation was successful or not.
     */
    void save(cl::CommandQueue &cmdQueue, cl::Image2D &image, std::string fileName, int height, int width);

    void saveBuffer(cl::Context &ctx,
                    cl::CommandQueue &cmdQueue,
                    cl::Buffer &image,
                    std::string fileName,
                    int height,
                    int width,
                    char *&buffer);

    /**
     * Creates a writable, empty image object in texture memory.
     * @param ctx An openCL context object.
     * @param height The desired height of the image object
     * @param width The desired width of the image object
     * @return An openCL Image2D object representing the texture memory.
     */
    cl::Image2D make(cl::Context &ctx, int height, int width);

    /**
     * Creates an openCL sampler object for sampling continuous values from an image with
     * hardware acceleration.
     * @param ctx An openCL context object.
     * @return An openCL sampler object.
     */
    cl::Sampler sampler(cl::Context &ctx);

} // namespace image

#endif
