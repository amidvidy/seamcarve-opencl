#include "numcy.h"

#include <stdlib.h>

int32_t* np_zero_array_int32(size_t length) {
    return calloc(length, sizeof(int32_t));
}

/* Currently allocates a contiguous block but still returns array
** of arrays, allowing double indirection to access elements.
** Caller can use optional pitch for direct access off of zero element.
*/
int32_t** np_zero_matrix_int32(size_t height, size_t width, size_t *pPitch) {
    size_t pitch = width; /* Can use this later to pad rows for alignment */
    
    int32_t *arr = calloc(height * pitch, sizeof(int32_t));
    if (!arr) return NULL;
    
    int32_t **arrarr = malloc(height * sizeof(int32_t*));
    if (arrarr) {
        for (int y = 0; y < height; y++) {
            arrarr[y] = &arr[y * pitch];
        }
        if (pPitch) *pPitch = pitch;
    } else free(arr);
    return arrarr;
}

/* WARNING: These just cheat and cast int32 to float (usually 32-bits too) */
float*  np_zero_array_float(size_t LEN) {
    return (float* ) np_zero_array_int32(LEN);
}

float** np_zero_matrix_float(size_t H, size_t W, size_t *pP) {
    return (float**) np_zero_matrix_float(H, W, pP);
}
