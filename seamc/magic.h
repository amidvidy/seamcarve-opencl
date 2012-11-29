#ifndef _MAGIC_H_
#define _MAGIC_H_

#include "numcy.h"

#include <wand/MagickWand.h>

MagickWand* MW_Blank(int H, int W, char *bgndStr);
MagickWand* MW_FromMatrix(bool isCOLOR, void** M, int H, int W);
void** MW_ToMatrix(bool isCOLOR, MagickWand *mw_in, int *pH, int *pW);

MagickWand* MW_Carve(bool isCOLOR, const MagickWand *mw_in, int newH, int newW);

//Image* IntMatrixToNewImage(int** M, int width, int height);

#endif // _MAGIC_H_
