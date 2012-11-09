/* Couple "drop-in" replacements for some NumPy calls. */

#include <stddef.h>
#include <stdint.h>

/*
** Can turn into something else later, and matrix
**   storage could change from current (convenient)
**   c-style array of array's.
** An important thing to add is GPU style array strides.
*/

int32_t*  np_zero_array_int32 (size_t length);
int32_t** np_zero_matrix_int32(size_t height, size_t width, size_t *pPitch);

/* WARNING: These just cheat and cast int32 to float (usually 32-bits too) */
float*    np_zero_array_float (size_t length);
float**   np_zero_matrix_float(size_t height, size_t width, size_t *pPitch);
