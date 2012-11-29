#include "numcy.h"

#include "magic.h"

#include <stdlib.h>
#include <stdio.h>

/* Note that calloc zeros the memory for us.  That's what it's used for here */

int32_t* np_zero_array_int32(size_t length)
{
    return (int32_t*) calloc(length, sizeof(int32_t));
}

/* Currently allocates a contiguous block but still returns array
 ** of arrays, allowing double indirection to access elements.
 ** Caller can use optional pitch for direct access off of zero element.
 */
int32_t** np_zero_matrix_int32(size_t height, size_t width, size_t *pPitch)
{
    size_t pitch = width; /* Can use this later to pad rows for alignment */
    
    // There is a single "backing array"...
    int32_t *arr = (int32_t*) calloc(height * pitch, sizeof(int32_t));
    if (!arr) return NULL;
    
    // The "y" axis is an array of pointers into that single backing array...
    int32_t **arrarr = (int32_t**) malloc(height * sizeof(int32_t*));
    if (arrarr) {
        for (int y = 0; y < height; y++) {
            arrarr[y] = &arr[y * pitch];
        }
        if (pPitch) *pPitch = pitch;
    } else free((void*) arr);
    // The backing array is in arrarr[0] (for direct pitched access & deallocation).
    return arrarr;
}

int32_t* np_free_array_int32(int32_t* A)
{
    if (A) free((void*) A);
    return NULL;
}

int32_t** np_free_matrix_int32(int32_t** M)
{
    if (M) {
        if (M[0]) free((void*) M[0]); // Free the backing array...
        free((void*) M); // ...then the indexing array.
    }
    return NULL;
}

/* WARNING: These just cheat and cast int32 to float (usually 32-bits too) */
float* np_zero_array_float(size_t LEN)
{
    return (float*) np_zero_array_int32(LEN);
}

float** np_zero_matrix_float(size_t H, size_t W, size_t *pP)
{
    return (float**) np_zero_matrix_int32(H, W, pP);
}

float* np_free_array_float(float* A)
{
    return (float*) np_free_array_int32((int32_t*) A);
}

float** np_free_matrix_float(float** M)
{
    return (float**) np_free_matrix_int32((int32_t**) M);
}

void DebugMatrix(void **IMG, int W, int H, const char* name, int remainWidth, bool isCOLOR)
{
    if (DBG_DUMPTXT) {
        fprintf(stderr, "IMG %s (%d x %d) %s\n", name, W, H, (isCOLOR) ? "RGBA" : "LUM");
        for (int y = 0; y < H; y++) {
            const void *pROW = IMG[y];
            fprintf(stderr, "ROW %d:", y);
            for (int x = 0; x < W; x++) {
                if (isCOLOR) {
                    F4_t tF4 = ((F4_t*) pROW)[x];
                    fprintf(stderr, " (%8.3f,%8.3f,%8.3f,%8.3f)", tF4.x, tF4.y, tF4.z, tF4.w);
                } else {
                    float tF = ((float*) pROW)[x];
                    fprintf(stderr, " %8.3f", tF);
                }
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
    }
    if (DBG_DUMPIMG) MW_DumpMatrix(IMG, H, W, name, isCOLOR);
}

