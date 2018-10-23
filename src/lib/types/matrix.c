/******************************************************************************
 * NAME:
 *  types/matrix.c
 * DESCRIPTION:
 *  Matrix data type implementation.
 *****************************************************************************/
#include "matrix.h"
#include <stdlib.h>
#include <string.h>

/******************************************************************************
 * Creation / destruction
 *****************************************************************************/

/**
 * Creates a matrix, given its size.
 * @param int width The matrix width (row size).
 * @param int height The matrix height (column size).
 * @return struct matrix_t* The matrix or NULL in case of failure.
 */
struct matrix_t *mat_make(int width, int height)
{
    int i;
    struct matrix_t *retVal;

    // Allocate space for the struct
    retVal = malloc(sizeof(struct matrix_t));
    if (retVal == NULL)
        return NULL;
    retVal->height = height;
    retVal->width = width;

    // Allocate space for the filter values
    // Not optimal!
    retVal->values = malloc(sizeof(double*) * retVal->height);
    if (retVal->values == NULL) {
        free(retVal);
        return NULL;
    }
    for (i = 0; i < retVal->height; i++) {
        retVal->values[i] = malloc(sizeof(double) * retVal->width);
        if (retVal->values[i] == NULL) {
            for (i -= 1; i >= 0; i--) {
                free(retVal->values[i]);
            }
            free(retVal->values);
            free(retVal);
            return NULL;
        }
    }

    return retVal;
}

/**
 * Destroys a matrix.
 * @param struct filter_t* The matrix to destroy.
 */
void mat_destroy(struct matrix_t *mat)
{
    int i;

    for (i = 0; i < mat->height; i++) {
        free(mat->values[i]);
    }
    free(mat->values);
    free(mat);
}

/******************************************************************************
 * I/O
 *****************************************************************************/

/**
 * Creates a matrix from a file.
 * @param FILE *file The input file.
 * @return struct matrix_t* The matrix or NULL.
 */
struct matrix_t* mat_makeFromFile(FILE *file)
{
    struct matrix_t *retVal;
    char buffer[1024], *value;
    double v, *vList;
    int vListCapacity, vListAmt, maxRowSize, rowSize, rowsAmt;
    int i, j, k;

    // Read file
    vListCapacity = 128;
    vListAmt = 0;
    maxRowSize = 0;
    rowsAmt = 0;
    vList = malloc(sizeof(double) * vListCapacity);
    while (fgets(buffer, 1024, file) != NULL) {
        value = strtok(buffer, " \t");
        rowSize = 0;
        while (value != NULL) {
            if(sscanf(value, "%lf", &v) != 1)
                break;
            rowSize++;
            vList[vListAmt++] = v;
            if (vListAmt == vListCapacity) {
                vListCapacity *= 2;
                vList = realloc(vList, sizeof(double) * vListCapacity);
            }
            value = strtok(NULL, " \t");
        }
        if (rowSize > maxRowSize) maxRowSize = rowSize;
        if (rowSize > 0) rowsAmt++;
    }

    // Allocate matrix
    retVal = mat_make(maxRowSize, rowsAmt);
    if (retVal == NULL) {
        free(vList);
        return NULL;
    }

    // Assign values
    k = 0;
    for (i = 0; i < rowsAmt; i++)
        for (j = 0; j < maxRowSize; j++) {
            retVal->values[i][j] = vList[k++];
        }
    free(vList);

    return retVal;
}
