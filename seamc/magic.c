#include "magic.h"

#include "numcy.h"
#include "seamc.h"

#include <wand/MagickWand.h>
#include <magick/MagickCore.h>

/* These 2 are handy converters tween MagickWand and MagickCore:
 MagickWand* NewMagickWandFromImage(const Image *image);
 Image*      GetImageFromMagickWand(const MagickWand *wand);

 Taking the tact of using Wand for convenience in most cases.

 Watch out for inconsistent width/height parameter order!
 */

/*
 * TODO: Check these for leaks & make "cleanup on error" code more consistent.
 */

MagickWand* MW_Blank(int H, int W, char *bgndStr)
{
    MagickWand* mw_ret = NewMagickWand();
    PixelWand* mw_pix = NewPixelWand();
    MagickBooleanType mw_ok = MagickFalse;
    
    if (mw_ret && mw_pix) {
        if (bgndStr) PixelSetColor(mw_pix, bgndStr);
        mw_ok = MagickNewImage(mw_ret, W, H, mw_pix);
    }
    if (mw_pix) mw_pix = DestroyPixelWand(mw_pix);
    // Should we always destroy or does MagickWand now own it pm success???
    if (mw_ok == MagickFalse) {
        if (mw_ret) mw_ret = DestroyMagickWand(mw_ret);
    }
    return mw_ret;
}

MagickWand* MW_FromMatrix(float** M, int H, int W)
{
    MagickBooleanType mw_ok;
    ExceptionInfo ex_info, *im_ex = &ex_info;
    Image* im_out = NULL;
    MagickWand* mw_out = NULL;
    
    if (!M || (H < 1) || (W < 1)) return NULL ;
    
    // Maybe should just make a fresh Wand and grab it's Image?  (Not sure how the backnforth works)
    ImageInfo* im_info = AcquireImageInfo();
    if (!im_info) return NULL ;
    MagickPixelPacket bkg;
    mw_ok = QueryMagickColor("white", &bkg, im_ex);
    if (mw_ok == MagickFalse) return NULL ;
    im_out = NewMagickImage(im_info, W, H, &bkg); // Can not use NULL for background, even if don't care!
    im_info = DestroyImageInfo(im_info);
    if (!im_out) return NULL ;
    
    mw_ok = ModifyImage(&im_out, im_ex); // Not sure what all this does but
// The for loop checks mw_ok
    for (int y = 0; (mw_ok != MagickFalse && (y < H)); y++) {
        // Push a row at a time, intensity/grayscale float
        mw_ok = ImportImagePixels(im_out, 0, y, W, 1, "I", FloatPixel, M[y]);
    }
    if (mw_ok == MagickFalse) {
        if (im_out) im_out = DestroyImage(im_out);
        return NULL ;
    }
    
    mw_out = NewMagickWandFromImage(im_out);
    // Do we free the Image or is it embedded into the Wand???
    return mw_out;
}

float** MW_ToMatrix(MagickWand *mw_in, int *pH, int *pW)
{
    MagickBooleanType mw_ok;
    ExceptionInfo ex_info, *im_ex = &ex_info;
    int h, w;
    float** M = NULL;
    
    if (!mw_in) return NULL ;
    
    h = MagickGetImageHeight(mw_in);
    w = MagickGetImageWidth(mw_in);
    if ((h < 1) || (w < 1)) return NULL ;
    
    // Docs seemed sparse on GetImageFrom MagickWand, however, it appears that the Image still "belongs"
    // to the Wand and we just get a reference to it (unless you Clone it I suppose).
    // If I destroy the image that was extracted from the Wand, then an Assertion fails later
    // when destroying the Wand itself.
    Image* im_in = GetImageFromMagickWand(mw_in);
    if (!im_in) return NULL ;
    mw_in = NULL; //Ensure Wand not used after this point
            
    M = np_zero_matrix_float(h, w, NULL ); // We just ignore pitch for now
    if (M == NULL ) return NULL ;
    
    //mw_ok = ModifyImage(&im_in, ex); // Not sure what all this does but
    //mw_ok = SetGrayscaleImage(im_in); // This method doesn't seem to exist!
    
    for (int y = 0; ((mw_ok != MagickFalse) && (y < h)); y++) {
        // Pop a row at a time, intensity/grayscale float
        mw_ok = ExportImagePixels(im_in, 0, y, w, 1, "I", FloatPixel, M[y], im_ex);
    }
    
    //if (im_in) im_in = DestroyImage(im_in); // IMPROPER if image came from a Wand
    
    if (mw_ok == MagickFalse) {
        if (M) M = np_free_matrix_float(M);
        h = 0;
        w = 0;
    }
    
    if (pH) *pH = h;
    if (pW) *pW = w;
    return M;
}

MagickWand* MW_Carve_Grey(const MagickWand *mw_in)
{
    MagickBooleanType mw_ok;
    MagickWand* mw_temp = NewMagickWand();
    if (!mw_temp) return NULL ;
    mw_ok = MagickAddImage(mw_temp, mw_in); // Clone the input image before altering it.
    if (mw_ok == MagickFalse) return NULL ;
    
    mw_ok = MagickSetImageType(mw_temp, GrayscaleType); // Will eliminate this, of course.
    //mw_ok = MagickSetImageType(mw_temp, MagickGetImageType(mw_temp)); //Some advise this after loading for some reason
    if (mw_ok == MagickFalse) return NULL ;
    
    int h, w;
    float** M_in = MW_ToMatrix(mw_temp, &h, &w); // Zero col & row indicate ALL col & rows
    mw_temp = DestroyMagickWand(mw_temp);
    
    float** M_out = np_zero_matrix_float(h, w, NULL );
    SEAMC_carveGrey(M_in, h, w, M_out, h, w - 10); // TODO: Apply new dimensions based on params
    M_in = np_free_matrix_float(M_in);
    
    mw_temp = MW_FromMatrix(M_out, h, w);
    M_out = np_free_matrix_float(M_out);
    
    return mw_temp;
}

// Demonstrates an iterator method of pixel access from example code using Wand only:
MagickWand* IntMatrixToNewImage(int** M, int img_width, int img_height)
{
    MagickBooleanType mw_ok;
    MagickWand *m_wand = MW_Blank(img_width, img_height, "white");
    //char hex[256]; // For specifying RGB as a string!
    PixelIterator *iterator = NewPixelIterator(m_wand);
    mw_ok = MagickTrue;
    for (int y = 0; ((mw_ok != MagickFalse) && (y < img_height)); y++) {
        size_t val; // Get the next row of the image as an array of PixelWands
        PixelWand **pixels = PixelGetNextIteratorRow(iterator, &val);
        
        // Copy the cprresponding row of the matrix into the image
        for (int x = 0; x < img_width; x++) {
            int gray = x % 256;
            //snprintf(hex, 255, "#%02x%02x%02x", gray, gray, gray);
            //PixelSetColor(pixels[x], hex);
            PixelSetRed(pixels[x], gray);
            PixelSetGreen(pixels[x], gray);
            PixelSetBlue(pixels[x], gray);
        }
        
        mw_ok = PixelSyncIterator(iterator); // Sync writes the pixels back to the m_wand
    }
    iterator = DestroyPixelIterator(iterator);
    return m_wand;
}
