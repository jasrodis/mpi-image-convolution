/******************************************************************************
 * NAME:
 *  types/image.h
 * DESCRIPTION:
 *  Image data type header file.
 *****************************************************************************/
#ifndef _TYPES_IMAGE
#define _TYPES_IMAGE

#include <stdio.h>

/******************************************************************************
 * Data structures
 *****************************************************************************/

struct image_t {            // Image
    int width;
    int height;
    int pixelSize;
    // Data
    unsigned char *data;
    // Data aligned as rows
    unsigned char **rows;
};

/******************************************************************************
 * Macros
 *****************************************************************************/

#define IMG_GET_PIXEL_PTR(img, height, width) \
    &(img->rows[height][width * img->pixelSize])
#define IMG_GET_PIXEL_BYTE(img, height, width, i) \
    img->rows[height][width * img->pixelSize + i]
#define IMG_SET_PIXEL_BYTE(img, height, width, i, newVal) \
    img->rows[height][width * img->pixelSize + i] = newVal
#define IMG_APPEND_PIXEL_BYTE(img, height, width, i, newVal) \
    img->rows[height][width * img->pixelSize + i] += newVal

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
struct image_t* img_make(int width, int height, int pixelSize);

/**
 * Destroys an image.
 * @param struct image_t* img The image to destroy.
 */
void img_destroy(struct image_t *img);

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
    int pixelSize);

/**
 * Writes an image.
 * @param struct image_t* img The image.
 * @param FILE *file The output file.
 */
void img_writeToFile(struct image_t *img, FILE *file);

/******************************************************************************
 * Operations
 *****************************************************************************/

/**
 * Crop an image.
 * @param struct image_t* img The image.
 * @param int width Width of the cropped imagein pixels.
 * @param int height Height of the cropped image in pixels.
 * @param int widthOffset The offset of the width.
 * @param int heightOffset The offset of the height.
 * @return struct image_t* The cropped image or NULL.
 */
struct image_t* img_crop(struct image_t *img, int width, int height,
    int widthOffset, int heightOffset);

/******************************************************************************
 * Distance
 *****************************************************************************/

/**
 * Gets the Euclidean distance between two images.
 * @param struct image_t *imgA The image.
 * @param struct image_t *imgB The image.
 * @return double The distance or -1 in case of error.
 */
double img_getDistance(struct image_t *imgA, struct image_t *imgB);

#endif
