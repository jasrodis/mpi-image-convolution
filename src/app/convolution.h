/******************************************************************************
 * NAME:
 *  convolution.h
 * DESCRIPTION:
 *  Image convolution header file.
 *****************************************************************************/
#ifndef _CONVOLUTION
#define _CONVOLUTION

#include <types/matrix.h>
#include <types/image.h>

/******************************************************************************
 * Filter normalization
 *****************************************************************************/

/**
 * Normalize the convolution filter.
 * @param struct matrix_t *mat The filter.
 * @return struct matrix_t* The resulting filter.
 */
struct matrix_t* conv_normalizeFilter(struct matrix_t *mat);

/******************************************************************************
 * Functionality
 *****************************************************************************/

/**
 * Partial running convolution. No filter normalization and other logic used.
 * @param struct image_t *inImg The input image.
 * @param int offsetRowIdx The offset (as row index).
 * @param int limit The limit (amount of rows).
 * @param struct image_t *outImg The output image.
 * @param struct matrix_t *normFilter The filter to apply. It is already
 *  normalized.
 */
void conv_runPartially(struct image_t *inImg, int offsetRowIdx, int limit,
    struct image_t *outImg, struct matrix_t *normFilter);

/**
 * Runs image convolution.
 * @param struct image_t *imp The image.
 * @param struct matrix_t *filter The filter to apply.
 * @return struct image_t* The resulting image or NULL in case of failure.
 */
struct image_t *conv_run(struct image_t *img, struct matrix_t *filter);

#endif
