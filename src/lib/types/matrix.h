/******************************************************************************
 * NAME:
 *  types/matrix.h
 * DESCRIPTION:
 *  Matrix data type header file.
 *****************************************************************************/
#ifndef _TYPES_MATRIX
#define _TYPES_MATRIX

#include <stdio.h>

/******************************************************************************
 * Data structures
 *****************************************************************************/

struct matrix_t {           // A matrix
    int width;
    int height;
    double **values;
};

/******************************************************************************
 * Creation / destruction
 *****************************************************************************/

/**
 * Creates a matrix, given its size.
 * @param int width The matrix width (row size).
 * @param int height The matrix height (column size).
 * @return struct matrix_t* The matrix or NULL in case of failure.
 */
struct matrix_t *mat_make(int width, int height);

/**
 * Destroys a matrix.
 * @param struct filter_t* The matrix to destroy.
 */
void mat_destroy(struct matrix_t *mat);

/******************************************************************************
 * I/O
 *****************************************************************************/

/**
 * Creates a matrix from a file.
 * @param FILE *file The input file.
 * @return struct matrix_t* The matrix or NULL.
 */
struct matrix_t* mat_makeFromFile(FILE *file);

#endif
