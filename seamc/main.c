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

void process(char *image_file) {
    MagickWand *magick_wand = NULL;
    MagickBooleanType status;
    int img_height, img_width;

    // Initialize ImageMagick
    MagickWandGenesis();

    magick_wand = NewMagickWand();

    status = MagickReadImage(magick_wand, image_file);

    if (status == MagickFalse) {
        ThrowWandException(magick_wand);
    }

    img_height = MagickGetImageWidth(magick_wand);
    img_width = MagickGetImageWidth(magick_wand);

    printf("img_height = %i\timage_width = %i\n", img_height, img_width);

    // Terminate ImageMagick
    MagickWandTerminus();
}

/**
 * Prints the expected command-line args.
 */
void usage(void) {
    printf("usage: <image.jpg>\n");
}

/**
 * Main entry point.
 */
int main(int argc, char *argv[]) {
    char *image_file;

    if (argc != 2) {
        printf("argc = %i\n", argc);
        usage();
        exit(-1);
    } else {
        image_file = argv[1];
        // DEBUG
        printf("%s\n", image_file);
        process(image_file);
    }
}


