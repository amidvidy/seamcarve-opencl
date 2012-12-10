#include "magic.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wand/MagickWand.h>

#include <fstream>
#include <iostream>
#include <exception>

#include <cuda_runtime.h>      // CUDA Runtime
#include <npp.h>               // CUDA NPP Definitions
#include "FreeImage.h"

#define ThrowWandException(wand) {                                      \
        char *description;                                              \
        ExceptionType severity;                                         \
        description=MagickGetException(wand,&severity);                 \
        (void) fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description); \
        description=(char *) MagickRelinquishMemory(description);       \
        exit(-1);                                                       \
    }

#define CUDIE(value) {                                                      \
    cudaError_t _m_cudaStat = value;                                        \
    if (_m_cudaStat != cudaSuccess) {                                       \
        fprintf(stderr, "Error %s at line %d in file %s\n",                 \
                cudaGetErrorString(_m_cudaStat), __LINE__, __FILE__);       \
        exit(1);                                                            \
    } }

bool g_bQATest = false;
int g_nDevice = -1;

void dumpNPPinfo()
{
	const char *sComputeCap[] = { "No CUDA Capable Device Found", "Compute 1.0",
			"Compute 1.1", "Compute 1.2", "Compute 1.3", "Compute 2.0",
			"Compute 2.1", "Compute 3.0", NULL };
	
	const NppLibraryVersion *libVer = nppGetLibVersion();
	NppGpuComputeCapability computeCap = nppGetGpuComputeCapability();
	
	printf("NPP Library Version %d.%d.%d\n", libVer->major, libVer->minor,
			libVer->build);
	
	if (computeCap != 0 && g_nDevice == -1) {
		printf("Using GPU <%s> with %d SM(s) with", nppGetGpuName(),
				nppGetGpuNumSMs(), sComputeCap[computeCap]);
		
		if (computeCap > 0) {
			printf(" %s\n", sComputeCap[computeCap]);
		} else {
			printf(" Unknown Compute Capabilities\n");
		}
	} else {
		printf("%s\n", sComputeCap[computeCap]);
	}
}

// Error handler for FreeImage library.
//  In case this handler is invoked, it throws an NPP exception.
extern "C" void FreeImageErrorHandler(FREE_IMAGE_FORMAT oFif,
		const char *zMessage)
{
	printf("FreeImageError: %s\n", zMessage);
	exit(EXIT_FAILURE);
}

void dumpFIInfo(FIBITMAP *rBitmap)
{
	FREE_IMAGE_COLOR_TYPE eType = FreeImage_GetColorType(rBitmap);
	
	std::string sType("Unknown");
	switch (eType) {
	case FIC_MINISWHITE:
		sType = "MINISWHITE";
		break;
	case FIC_MINISBLACK:
		sType = "MINISBLACK";
		break;
	case FIC_RGB:
		sType = "RGB";
		break;
	case FIC_PALETTE:
		sType = "PALETTE";
		break;
	case FIC_RGBALPHA:
		sType = "RGBALPHA";
		break;
	case FIC_CMYK:
		sType = "CMYK";
		break;
	}
	
	::printf("Size %dx%d, Pitch: %d, Depth: %d, Type: %s\n", //
			FreeImage_GetWidth(rBitmap), FreeImage_GetHeight(rBitmap), //
			FreeImage_GetPitch(rBitmap), FreeImage_GetBPP(rBitmap), //
			sType.c_str());
}

void processFreeImage(const char *in_file, const char *out_file, //
		int out_width, int out_height, //
		bool isCOLOR = true, bool drawLINE = false)
{
	try {
		FreeImage_SetOutputMessage(FreeImageErrorHandler); // FreeImage error handler
				
		FREE_IMAGE_FORMAT eFormat = FreeImage_GetFileType(in_file);
		if (eFormat == FIF_UNKNOWN) { // If unknown then guess
			eFormat = FreeImage_GetFIFFromFilename(in_file);
		}
		
		FIBITMAP *pBitmapA = FreeImage_Load(eFormat, in_file);
		if (!pBitmapA) return;
		dumpFIInfo(pBitmapA);
		
		FIBITMAP *pBitmap = FreeImage_ConvertTo32Bits(pBitmapA);
		FreeImage_Unload(pBitmapA);
		pBitmapA = NULL;
		dumpFIInfo(pBitmap);
		
		unsigned int nImageWidth = FreeImage_GetWidth(pBitmap);
		unsigned int nImageHeight = FreeImage_GetHeight(pBitmap);
		unsigned int nSrcPitch = FreeImage_GetPitch(pBitmap);
		unsigned char *pSrcData = FreeImage_GetBits(pBitmap);
		
		int nSrcPitchCUDA;
		Npp8u *pSrcImageCUDA = nppiMalloc_8u_C4(nImageWidth, nImageHeight,
				&nSrcPitchCUDA);
		if (!pSrcImageCUDA) return;
		
		CUDIE( cudaMemcpy2D(pSrcImageCUDA, nSrcPitchCUDA, //
				pSrcData, nSrcPitch,//
				nImageWidth * 4, nImageHeight,//
				cudaMemcpyHostToDevice));
		FreeImage_Unload(pBitmap);
		pBitmap = NULL;
		
		// define size of the box filter
		const NppiSize oMaskSize = { 7, 7 };
		const NppiPoint oMaskAchor = { 0, 0 };
		// compute maximal result image size
		const NppiSize oSizeROI = { nImageWidth, nImageHeight };
		//{ (nImageWidth - (oMaskSize.width - 1)),
		//(nImageHeight - (oMaskSize.height - 1)) };
		// allocate result image memory
		int nDstPitchCUDA;
		Npp8u *pDstImageCUDA = nppiMalloc_8u_C4(oSizeROI.width, oSizeROI.height,
				&nDstPitchCUDA);
		if (!pDstImageCUDA) return;
		nppiFilterBox_8u_C4R(pSrcImageCUDA, nSrcPitchCUDA, //
				pDstImageCUDA, nDstPitchCUDA, //
				oSizeROI, oMaskSize, oMaskAchor);
		// create result image storage using FreeImage so we can easily save
		FIBITMAP *pResultBitmap = FreeImage_Allocate(oSizeROI.width,
				oSizeROI.height, 32 /* bits per pixel */);
		if (!pResultBitmap) return;
		
		unsigned int nResultPitch = FreeImage_GetPitch(pResultBitmap);
		unsigned char *pResultData = FreeImage_GetBits(pResultBitmap);
		
		CUDIE( cudaMemcpy2D(pResultData, nResultPitch, //
				pDstImageCUDA, nDstPitchCUDA,//
				oSizeROI.width * 4, oSizeROI.height,// width is bytes
				cudaMemcpyDeviceToHost));
		
		eFormat = FreeImage_GetFIFFromFilename(out_file);
		if (eFormat == FIF_UNKNOWN) eFormat = FIF_PBM;
		bool bSuccess = FreeImage_Save(eFormat, pResultBitmap, out_file,
				0) == TRUE;
		dumpFIInfo(pResultBitmap);
		FreeImage_Unload(pResultBitmap);
		pResultBitmap = NULL;
		
		nppiFree(pSrcImageCUDA);
		nppiFree(pDstImageCUDA);
		
		cudaDeviceReset();
	} catch (...) {
		::printf("Aborting.\n");
		exit(EXIT_FAILURE);
	}
	
}

void processMagick(const char *in_file, const char *out_file, //
		int out_width, int out_height, //
		bool isCOLOR = true, bool drawLINE = false)
{
	MagickWand *magick_wand = NULL;
	MagickBooleanType status;
	int img_height, img_width;
	
	// Initialize ImageMagick
	MagickWandGenesis();
	
	// Load image from file
	magick_wand = NewMagickWand();
	status = MagickReadImage(magick_wand, in_file);
	if (status == MagickFalse) ThrowWandException(magick_wand);
	
	// Output basic info
	img_height = MagickGetImageHeight(magick_wand);
	img_width = MagickGetImageWidth(magick_wand);
	if (out_height <= 0) out_height += img_height;
	if (out_width <= 0) out_width += img_width;
	
	printf("(w x h) IN: %i x %i  OUT: %i x %i\n", img_width, img_height,
			out_width, out_height);
	
	MagickWand* mw_out = MW_Carve(magick_wand, out_height, out_width, isCOLOR,
			drawLINE); // color, lines
	if (mw_out) {
		status = MagickWriteImage(mw_out, out_file);
		if (DBG_DUMPIMG) {
			status = MagickWriteImage(mw_out, "out.jpg");
			status = MagickWriteImage(mw_out, "out.tif");
			status = MagickWriteImage(mw_out, "out.ppm");
		}
		
		if (status == MagickFalse) ThrowWandException(magick_wand);
		
		mw_out = DestroyMagickWand(mw_out);
	} else fprintf(stderr, "Error Carving Image.\n");
	
	// Tidy up
	magick_wand = DestroyMagickWand(magick_wand);
	
	// Terminate ImageMagick
	MagickWandTerminus();
}

/**
 * Prints the expected command-line args.
 */
void usage(void)
{
	printf(
			"usage: <image.{jpg,png,tif,...}> [<outimg.xyz> [<new-width> [<new-height>]]]\n");
	printf(
			"     : new-width/height may be a negative number to indicate relative shrink.\n");
}

/**
 * Main entry point.
 */
int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("argc = %i\n", argc);
		usage();
		exit(-1);
	} else {
		bool isCOLOR = (strcmp(argv[0], "seamc_grey") != 0)
				&& (strcmp(argv[0], "linec_grey") != 0);
		bool drawLINE = (strcmp(argv[0], "linec_grey") == 0)
				|| (strcmp(argv[0], "linec") == 0);
		
		char inFile[1024], outFile[1024];
		strncpy(inFile, (argc > 1) ? argv[1] : "in.tif", 1024); // This case doesn't happen.
		strncpy(outFile, (argc > 2) ? argv[2] : "out.tif", 1024);
		int new_width = (argc > 3) ? atoi(argv[3]) : -10;
		int new_height = (argc > 4) ? atoi(argv[4]) : 0;
		printf("%s -> %s (%d x %d)\n", inFile, outFile, new_width, new_height);
		
		processFreeImage(inFile, outFile, new_width, new_height, isCOLOR,
				drawLINE);
	}
}

