#include "magic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wand/MagickWand.h>

#define ThrowWandException(wand) {                                      \
        char *description;                                              \
        ExceptionType severity;                                         \
        description=MagickGetException(wand,&severity);                 \
        (void) fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description); \
        description=(char *) MagickRelinquishMemory(description);       \
        exit(-1);                                                       \
    }

void process(const char *in_file, const char *out_file, //
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
    
    printf("(w x h) IN: %i x %i  OUT: %i x %i\n", img_width, img_height, out_width, out_height);
    
    MagickWand* mw_out = MW_Carve(magick_wand, out_height, out_width, isCOLOR, drawLINE); // color, lines
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
    printf("usage: <image.{jpg,png,tif,...}> [<outimg.xyz> [<new-width> [<new-height>]]]\n");
    printf("     : new-width/height may be a negative number to indicate relative shrink.\n");
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
        bool isCOLOR = (strcmp(argv[0], "seamc_grey") != 0) && (strcmp(argv[0], "linec_grey") != 0);
        bool drawLINE = (strcmp(argv[0], "linec_grey") == 0) || (strcmp(argv[0], "linec") == 0);

        char inFile[1024], outFile[1024];
        strncpy(inFile, (argc > 1) ? argv[1] : "in.tif", 1024); // This case doesn't happen.
        strncpy(outFile, (argc > 2) ? argv[2] : "out.tif", 1024);
        int new_width = (argc > 3) ? atoi(argv[2]) : -10;
        int new_height = (argc > 4) ? atoi(argv[3]) : 0;
        printf("%s -> %s (%d x %d)\n", inFile, outFile, new_width, new_height);

        process(inFile, outFile, new_width, new_height, isCOLOR, drawLINE);
    }
}

