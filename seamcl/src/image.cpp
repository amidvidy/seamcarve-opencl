#include "image.hpp"

// SeamCL
#include "mem.hpp"

#include <iostream>

// Functions relating to image handling
namespace image {

    /**
     * Loads an image from a file into device texture memory.
     * @param ctx An openCL context object.
     * @param fileName The image file to use.
     * @param height An int ref to read the height into for later use.
     * @param width An int ref to read the width into for later use.
     * @return An openCL image2D object representing the image on the device.
     */
    cl::Image2D load(cl::Context &ctx, std::string fileName, int &height, int &width) {
        FREE_IMAGE_FORMAT format = FreeImage_GetFileType(fileName.c_str(), 0);
        FIBITMAP *image = FreeImage_Load(format, fileName.c_str());

        image = FreeImage_ConvertTo32Bits(image);
        width = FreeImage_GetWidth(image);
        height = FreeImage_GetHeight(image);

        char buffer[width * height * 4];

        memcpy(buffer, FreeImage_GetBits(image), width * height * 4);

        FreeImage_Unload(image);

        cl_int errNum;
        cl::ImageFormat imageFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);

        cl::Image2D img =  cl::Image2D(ctx,
                                       (cl_mem_flags) CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                       imageFormat,
                                       width,
                                       height,
                                       0,
                                       buffer,
                                       &errNum);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error loading image: " << fileName << std::endl;
            exit(-1);
        }

        return img;
    }

    cl::Buffer loadBuffer(cl::Context &ctx,
                          cl::CommandQueue &cmdQueue,
                          std::string fileName,
                          int &height,
                          int &width,
                          char *&bytes) {
        FREE_IMAGE_FORMAT format = FreeImage_GetFileType(fileName.c_str(), 0);
        FIBITMAP *image = FreeImage_Load(format, fileName.c_str());

        image = FreeImage_ConvertTo32Bits(image);
        width = FreeImage_GetWidth(image);
        height = FreeImage_GetHeight(image);

        size_t numBytes = width * height * 4;
        std::cout << "numBytes in image = " << numBytes << std::endl;
        // Lets try comparing these buffers manually.
        bytes = new char[numBytes];
        char img[numBytes];
        memcpy(img, FreeImage_GetBits(image), numBytes);
        //memcpy(bytes, FreeImage_GetBits(image), numBytes);
        memcpy(bytes, img, numBytes);

        //for (int i = 0; i < numBytes; ++i) {
        //  if (bytes[i] != img[i]) {
        //      std::cerr << "Difference!" << std::endl;
        //  }
        //}

        FreeImage_Unload(image);

        cl::Buffer buff = mem::buffer(ctx, cmdQueue, width * height * 4);
        
        mem::write(ctx, cmdQueue, img, buff, numBytes);
        //mem::write(ctx, cmdQueue, bytes, buff, numBytes);

        return buff;
    }

    void saveBuffer(cl::Context ctx,
                    cl::CommandQueue &cmdQueue,
                    cl::Buffer &image,
                    std::string fileName,
                    int height,
                    int width,
                    char *&outBuffer) {
        char buffer[width * height * 4];
        outBuffer = new char[width * height * 4];
        mem::read(ctx, cmdQueue, buffer, image, width * height * 4);

        FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(fileName.c_str());
        FIBITMAP *bitmap = FreeImage_ConvertFromRawBits((BYTE*)buffer,
                                                        width,
                                                        height,
                                                        width * 4,
                                                        32,
                                                        0xFF000000,
                                                        0x00FF0000,
                                                        0x0000FF00,
                                                        FALSE);

        if (FreeImage_Save(format, bitmap, fileName.c_str()) != TRUE) {
            std::cerr << "Error writing output image: " << fileName << std::endl;
            exit(-1);
        }

    }

    /**
     * Saves the contents of an image object to disk.
     * @param cmdQueue An openCL commandQueue object.
     * @param image The image object to read data from.
     * @param fileName The file in which to store the resulting image.
     * @param height The height of the image.
     * @param width The width of the image.
     * @return Whether the operation was successful or not.
     */
    void save(cl::CommandQueue &cmdQueue, cl::Image2D &image, std::string fileName, int height, int width) {
        cl_int errNum;
        char buffer[width * height * 4];
        cl::size_t<3> origin;
        origin.push_back(0);
        origin.push_back(0);
        origin.push_back(0);
        cl::size_t<3> region;
        region.push_back(width);
        region.push_back(height);
        region.push_back(1);

        errNum = cmdQueue.enqueueReadImage(image,
                                           CL_TRUE,
                                           origin,
                                           region,
                                           0, // row pitch is 0
                                           0, // slice pitch is 0
                                           buffer,
                                           NULL, // no events
                                           NULL);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error reading image data." << std::endl;
            exit(-1);
        }

        FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(fileName.c_str());
        FIBITMAP *bitmap = FreeImage_ConvertFromRawBits((BYTE*)buffer,
                                                        width,
                                                        height,
                                                        width * 4,
                                                        32,
                                                        0xFF000000,
                                                        0x00FF0000,
                                                        0x0000FF00,
                                                        FALSE);

        if (FreeImage_Save(format, bitmap, fileName.c_str()) != TRUE) {
            std::cerr << "Error writing output image: " << fileName << std::endl;
            exit(-1);
        }

    }

    /**
     * Creates a writable, empty image object in texture memory.
     * @param ctx An openCL context object.
     * @param height The desired height of the image object
     * @param width The desired width of the image object
     * @return An openCL Image2D object representing the texture memory.
     */
    cl::Image2D make(cl::Context &ctx, int height, int width) {
        cl_int errNum;

        cl::ImageFormat imageFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);

        cl::Image2D img = cl::Image2D(ctx,
                                      (cl_mem_flags) CL_MEM_READ_WRITE,
                                      imageFormat,
                                      width,
                                      height,
                                      0,
                                      NULL,
                                      &errNum);
        if (errNum != CL_SUCCESS) {
            std::cerr << "Error creating image." << std::endl;
            exit(-1);
        }
        return img;
    }

    /**
     * Creates an openCL sampler object for sampling continuous values from an image with
     * hardware acceleration.
     * @param ctx An openCL context object.
     * @return An openCL sampler object.
     */
    cl::Sampler sampler(cl::Context &ctx) {
        cl_int errNum;

        cl::Sampler sampler = cl::Sampler(ctx,
                                          CL_FALSE, // Non-normalized coordinates
                                          CL_ADDRESS_CLAMP_TO_EDGE,
                                          CL_FILTER_NEAREST,
                                          &errNum);

        if (errNum != CL_SUCCESS) {
            std::cerr << "Error creating CL sampler object." << std::endl;
            exit(-1);
        }

        return sampler;

    }

} // namespace image
