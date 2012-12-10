#include "numcy.h"

#include "magic.h"

#include <stdlib.h>
#include <stdio.h>

/* Currently allocates a contiguous block but still returns array
 ** of arrays, allowing double indirection to access elements.
 ** Caller can use optional pitch for direct access off of zero element.
 */
void** np_new_matrix_x(size_t height, size_t width, size_t *pPitch, size_t sz,
		bool doZero)
{
	size_t pitch = width; /* Can use this later to pad rows for alignment */
	size_t szBytes = height * pitch * sz;
	
	// There is a single "backing array"...
	char *arr = (char*) ((doZero) ? ::calloc(szBytes, 1) : ::malloc(szBytes));
	if (!arr) return NULL;
	
	// The "y" axis is an array of pointers into that single backing array...
	void **arrarr = (void**) ::malloc(height * sizeof(void*));
	if (arrarr) {
		for (int y = 0; y < height; y++) {
			arrarr[y] = arr + (y * pitch * sz); // Uses char* to get bytewise ptr addition
		}
		if (pPitch) *pPitch = pitch;
	} else ::free(arr);
	// The backing array is in arrarr[0] (for direct pitched access & deallocation).
	return arrarr;
}

void** np_free_matrix_x(void** M)
{
	if (M) {
		if (M[0]) ::free(M[0]); // Free the backing array...
		::free((void*) M); // ...then the indexing array.
	}
	return NULL;
}

void DebugMatrix(void **IMG, int W, int H, const char* name, int remainWidth,
		bool isCOLOR)
{
	if (DBG_DUMPTXT) {
		fprintf(stderr, "IMG %s (%d x %d) %s\n", name, W, H,
				(isCOLOR) ? "RGBA" : "LUM");
		for (int y = 0; y < H; y++) {
			const void *pROW = IMG[y];
			fprintf(stderr, "ROW %d:", y);
			for (int x = 0; x < W; x++) {
				if (isCOLOR) {
					F4_t tF4 = ((F4_t*) pROW)[x];
					fprintf(stderr, " (%8.3f,%8.3f,%8.3f,%8.3f)", tF4.x, tF4.y,
							tF4.z, tF4.w);
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

