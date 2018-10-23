/******************************************************************************
 * NAME:
 *  convolution.c
 * DESCRIPTION:
 *  Image convolution implementation.
 *****************************************************************************/
#include "convolution.h"
#include <stdlib.h>
#include <stdio.h>

/******************************************************************************
 * Filter normalization
 *****************************************************************************/

/**
 * Normalize the convolution filter.
 * @param struct matrix_t *mat The filter.
 * @return struct matrix_t* The resulting filter.
 */
struct matrix_t* conv_normalizeFilter(struct matrix_t *mat)
{
    int i, j;
    double sum;
    struct matrix_t *retVal;

    // Make new image
    retVal = mat_make(mat->width, mat->height);
    if (retVal == NULL) {
        return NULL;
    }

    // Find max
    sum = 0;
    for (i = 0; i < mat->height; i++)
        for (j = 0; j < mat->width; j++)
            sum += mat->values[i][j];

    // Normalize
    for (i = 0; i < mat->height; i++)
        for (j = 0; j < mat->width; j++)
            if (sum == 0)
                retVal->values[i][j] = mat->values[i][j] / sum;
            else
                retVal->values[i][j] = mat->values[i][j];

    return retVal;
}

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
    struct image_t *outImg, struct matrix_t *normFilter)
{
    int rowIdx;
    int pixelIdx, byteIdx;
    int inRowIdx, inPixelIdx;
    int s;
    int p, q;
    int *accs;

    // Prepare
    s = (normFilter->width - 1) / 2;
    accs = malloc(sizeof(int) * inImg->pixelSize);

    // Go through each row
    for (rowIdx = offsetRowIdx; rowIdx < inImg->height
        && rowIdx < offsetRowIdx + limit; rowIdx++)
        // For each pixel in the row
        for (pixelIdx = 0; pixelIdx < inImg->width; pixelIdx++) {
            // Initialize pixel bytes
            for (byteIdx = 0; byteIdx < inImg->pixelSize; byteIdx++)
                accs[byteIdx] = 0;

            // Building the sums
            for (p = -s; p <= s; p++) {
                inRowIdx = rowIdx - p;
                if (inRowIdx >= inImg->height || inRowIdx < 0)
                    continue;
                for (q = -s; q <= s; q++) {
                    inPixelIdx = pixelIdx - q;
                    if (inPixelIdx >= inImg->width || inPixelIdx < 0)
                        continue;

                    // For each byte
                    for (byteIdx = 0; byteIdx < inImg->pixelSize; byteIdx++)
                        accs[byteIdx] +=
                            IMG_GET_PIXEL_BYTE(
                                inImg, inRowIdx, inPixelIdx, byteIdx
                            ) * normFilter->values[p + s][q + s];
                }
            }

            // Set pixel bytes
            for (byteIdx = 0; byteIdx < inImg->pixelSize; byteIdx++) {
                accs[byteIdx] = (accs[byteIdx] > 255) ? 255
                    : (accs[byteIdx] < 0) ? 0
                        : accs[byteIdx];
                IMG_SET_PIXEL_BYTE(
                    outImg, rowIdx, pixelIdx, byteIdx, accs[byteIdx]
                );
            }
        }

    // Clean up
    free(accs);
}

/**
 * Runs image convolution.
 * @param struct image_t *imp The image.
 * @param struct matrix_t *filter The filter to apply.
 * @return struct image_t* The resulting image or NULL in case of failure.
 */
struct image_t *conv_run(struct image_t *img, struct matrix_t *filter)
{
    struct image_t *retVal;
    struct matrix_t *normFilter;

    // Normalize the filter
    normFilter = conv_normalizeFilter(filter);

    // Make output image
    retVal = img_make(img->width, img->height, img->pixelSize);
    if (retVal == NULL) {
        return NULL;
    }

    // Run the partial call
    conv_runPartially(img, 0, img->height, retVal, normFilter);

    // Clean up
    mat_destroy(normFilter);

    return retVal;
}
