#include "magic.h"

#include <stdio.h>
#include <stdlib.h>
#include <wand/MagickWand.h>

#define ThrowWandException(wand) {                                      \
        char *description;                                              \
        ExceptionType severity;                                         \
        description=MagickGetException(wand,&severity);                 \
        (void) fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description); \
        description=(char *) MagickRelinquishMemory(description);       \
        exit(-1);                                                       \
    }

void process(char *image_file, int out_width, int out_height)
{
    MagickWand *magick_wand = NULL;
    MagickBooleanType status;
    int img_height, img_width;
    
    // Initialize ImageMagick
    MagickWandGenesis();
    
    // Load image from file
    magick_wand = NewMagickWand();
    status = MagickReadImage(magick_wand, image_file);
    if (status == MagickFalse) ThrowWandException(magick_wand);
    
    // Output basic info
    img_height = MagickGetImageHeight(magick_wand);
    img_width = MagickGetImageWidth(magick_wand);
    if (out_height <= 0) out_height += img_height;
    if (out_width <= 0) out_width += img_width;

    printf("(w x h) IN: %i x %i  OUT: %i x %i\n", img_width, img_height, out_width, out_height);
    
    // Carve it up, grey-style
    MagickWand* mw_out = MW_Carve_Grey(magick_wand, out_height, out_width);
    if (mw_out) {
        status = MagickWriteImage(mw_out, "out.jpg");
        status = MagickWriteImage(mw_out, "out.tif");
	status = MagickWriteImage(mw_out, "out.ppm");

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
    printf("usage: <image.{jpg,png,tif,...}> [<new-width> <new-height>]\n");
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
        char *image_file = argv[1];
	int new_width = (argc > 2) ? atoi(argv[2]) : -10;
	int new_height = (argc > 3) ? atoi(argv[3]) : 0;
        printf("%s %d %d\n", image_file, new_width, new_height);
        process(image_file, new_width, new_height);
    }
}

