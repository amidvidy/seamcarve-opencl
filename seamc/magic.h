#ifndef _MAGIC_H_
#define _MAGIC_H_

#include "numcy.h"

#include <wand/MagickWand.h>

MagickWand* MW_Blank(int H, int W, char *bgndStr);
MagickWand* MW_FromMatrix(float** M, int H, int W);
float** MW_ToMatrix(MagickWand *mw_in, int *pH, int *pW);

MagickWand* MW_Carve_Grey(const MagickWand *mw_in, int newH, int newW);

//Image* IntMatrixToNewImage(int** M, int width, int height);

#endif // _MAGIC_H_
