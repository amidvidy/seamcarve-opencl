#ifndef _MAGIC_H_
#define _MAGIC_H_

#include "numcy.h"

#include <wand/MagickWand.h>

MagickWand* MW_Blank(int H, int W, char *bgndStr);
MagickWand* MW_FromMatrix(void** M, int H, int W, bool isCOLOR = true);
void** MW_ToMatrix(MagickWand *mw_in, int *pH, int *pW, bool isCOLOR = true);

MagickWand* MW_Carve(const MagickWand *mw_in, int newH, int newW, bool isCOLOR = true, bool drawLINE = false);

void MW_DumpMatrix(void** M, int H, int W, const char*fileName, bool isCOLOR = true);

//Image* IntMatrixToNewImage(int** M, int width, int height);

#endif // _MAGIC_H_
