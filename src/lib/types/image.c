/******************************************************************************
 * NAME:
 *  types/image.c
 * DESCRIPTION:
 *  Image data type implementation.
 *****************************************************************************/
#include "image.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/******************************************************************************
 * Creation / destruction
 *****************************************************************************/

/**
 * Creates an empty image.
 * @param FILE *file The input file.
 * @param int width Width in pixels.
 * @param int height Height in pixels.
 * @param int pixelSize The size of a pixel in number of bytes.
 * @return struct image_t* The image or NULL.
 */
struct image_t* img_make(int width, int height, int pixelSize)
{
    int i, j;
    struct image_t *retVal;

    // Allocate space
    retVal = malloc(sizeof(struct image_t));
    if (retVal == NULL) {
        return NULL;
    }
    retVal->height = height;
    retVal->width = width;
    retVal->pixelSize = pixelSize;

    // Allocate space for the data
    retVal->data = malloc(sizeof(unsigned char*) * height * width * pixelSize);
    if (retVal->data == NULL) {
        free(retVal);
        return NULL;
    }

    // Align data into rows
    retVal->rows = malloc(sizeof(unsigned char*) * height);
    if (retVal->rows == NULL) {
        free(retVal->data);
        free(retVal);
        return NULL;
    }
    for (i = 0; i < height; i++) {
        retVal->rows[i] = &(retVal->data[i * width * pixelSize]);

        // Initialize
        for (j = 0; j < pixelSize * width; j++)
            retVal->rows[i][j] = 0;
    }

    return retVal;
}

/**
 * Destroys an image.
 * @param struct image_t* img The image to destroy.
 */
void img_destroy(struct image_t *img)
{
    free(img->data);
    free(img->rows);
    free(img);
}

/******************************************************************************
 * I/O
 *****************************************************************************/

/**
 * Creates an image from a file.
 * @param FILE *file The input file.
 * @param int width Width in pixels.
 * @param int height Height in pixels.
 * @param int pixelSize The size of a pixel in number of bytes.
 * @return struct image_t* The image or NULL.
 */
struct image_t* img_makeFromFile(FILE *file, int width, int height,
    int pixelSize)
{
    struct image_t *img;
    int i;

    // Make image
    img = img_make(width, height, pixelSize);
    if (img == NULL) {
        return NULL;
    }

    // Read file
    for (i = 0; i < img->height; i++) {
        if (fread(img->rows[i], pixelSize, img->width, file) !=
            (size_t) img->width) {
            img_destroy(img);
            return NULL;
        }
    }


    return img;
}

/**
 * Writes an image.
 * @param struct image_t* img The image.
 * @param FILE *file The output file.
 */
void img_writeToFile(struct image_t *img, FILE *file)
{
    int i;

    // Write file
    for (i = 0; i < img->height; i++) {
        fwrite(img->rows[i], img->pixelSize, img->width, file);
    }
}

/******************************************************************************
 * Operations
 *****************************************************************************/

/**
 * Crop an image.
 * @param struct image_t* img The image.
 * @param int width Width of the cropped image in pixels.
 * @param int height Height of the cropped image in pixels.
 * @param int widthOffset The offset of the width.
 * @param int heightOffset The offset of the height.
 * @return struct image_t* The cropped image or NULL.
 */
struct image_t* img_crop(struct image_t *img, int width, int height,
    int widthOffset, int heightOffset)
{
    int i, newI;
    struct image_t *retVal;

    // Check params
    if (widthOffset + width > img->width
        || heightOffset + height > img->height) return NULL;

    // Make new image
    retVal = img_make(width, height, img->pixelSize);
    if (retVal == NULL) {
        return NULL;
    }

    // Read from old image
    newI = 0;
    for (i = heightOffset; i < heightOffset + height; i++) {
        memcpy(
            retVal->rows[newI++],
            img->rows[i] + (img->pixelSize * widthOffset),
            img->pixelSize * width
        );
    }

    return retVal;
}

/******************************************************************************
 * Distance
 *****************************************************************************/

/**
 * Gets the Euclidean distance between two images.
 * @param struct image_t *imgA The image.
 * @param struct image_t *imgB The image.
 * @return double The distance or -1 in case of error.
 */
double img_getDistance(struct image_t *imgA, struct image_t *imgB)
{
    int i, j, k;
    double retVal;

    // Check images properties are the same
    if (imgA->width != imgB->width || imgA->height != imgB->height
        || imgA->pixelSize != imgB->pixelSize)
        return -1;

    // Go through all bytes
    retVal = 0.0;
    for (i = 0; i < imgA->height; i++)
        for (j = 0; j < imgA->width; j++)
            for (k = 0; k < imgA->pixelSize; k++)
                retVal += pow(
                    imgA->rows[i][j * imgA->pixelSize + k]
                    - imgB->rows[i][j * imgA->pixelSize + k],
                    2
                );

    // Return
    return sqrt(retVal);
}

