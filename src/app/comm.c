/******************************************************************************
 * NAME:
 *  comm.c
 * DESCRIPTION:
 *  Communications module implementation.
 *****************************************************************************/
#include "comm.h"
#include <mpi.h>

#define MY_COMM MPI_COMM_WORLD

/******************************************************************************
 * Start / stop the communication
 *****************************************************************************/

/**
 * Starts the communication session.
 * @param int *argc The arguments count (pointer).
 * @param char ***argv The list of arguments (pointer).
 * @return int The rank (id) of the process.
 */
int comm_start(int *argc, char ***argv)
{
    int retVal;

    // Create the communicator
    MPI_Init(argc, argv);

    // Get the rank
    MPI_Comm_rank(MY_COMM, &retVal);

    return retVal;
}

/**
 * Stop the communication session.
 */
void comm_stop()
{
    MPI_Finalize();
}

/******************************************************************************
 * Timing
 *****************************************************************************/

/**
 * Returns the wall clock time in a parallelism-safe way.
 * @return double The wall clock time.
 */
double comm_wTime()
{
    return MPI_Wtime();
}

/******************************************************************************
 * Information
 *****************************************************************************/

/**
 * Returns the size of the pool.
 * @return int The size of the pool.
 */
int comm_getSize()
{
    int retVal;

    // Actually get value
    MPI_Comm_size(MY_COMM, &retVal);

    return retVal;
}

/**
 * Returns the rank of the process.
 * @return int The rank of the process.
 */
int comm_getRank()
{
    int retVal;

    // Actually get value
    MPI_Comm_rank(MY_COMM, &retVal);

    return retVal;
}

/******************************************************************************
 * Matrix transferring
 *****************************************************************************/

/**
 * Broadcast a matrix to all processes of the communicator.
 * @param struct matrix_t *inMat The matrix to broadcast. Set to NULL for all
 *  non-root processes.
 * @return struct matirx_t* The broadcasted matrix. NULL in case of failure.
 */
struct matrix_t* comm_broadcastMatrix(struct matrix_t *inMat)
{
    int i;
    int rank;
    int height, width;
    struct matrix_t *mat = NULL;

    // Get the rank
    MPI_Comm_rank(MY_COMM, &rank);

    // Send the size
    if (rank == 0) {
        width = inMat->width;
        height = inMat->height;
    }
    MPI_Bcast(&width, 1, MPI_INT, 0, MY_COMM);
    MPI_Bcast(&height, 1, MPI_INT, 0, MY_COMM);

    // Allocate space for new matrix (non-root processes)
    if (rank != 0)
        mat = mat_make(width, height);
    else
        mat = inMat;

    // Broadcast data line by line
    for (i = 0; i < height; i++) {
        MPI_Bcast(mat->values[i], width, MPI_DOUBLE, 0, MY_COMM);
    }

    return mat;
}

/******************************************************************************
 * Image transferring
 *****************************************************************************/

/**
 * Broadcast an empty image (with proper dimensions though) to all processes of
 *  the communicator.
 * @param struct image_t *inImg The image to broadcast. Set to NULL for all
 *  non-root processes.
 * @return struct image_t* The broadcasted image. NULL in case of failure.
 */
struct image_t* comm_broadcastEmptyImg(struct image_t *inImg)
{
    int rank;
    int height, width, pixelSize;
    struct image_t *img = NULL;

    // Get the rank
    MPI_Comm_rank(MY_COMM, &rank);

    // Send the size
    if (rank == 0) {
        width = inImg->width;
        height = inImg->height;
        pixelSize = inImg->pixelSize;
    }
    MPI_Bcast(&width, 1, MPI_INT, 0, MY_COMM);
    MPI_Bcast(&height, 1, MPI_INT, 0, MY_COMM);
    MPI_Bcast(&pixelSize, 1, MPI_INT, 0, MY_COMM);

    // Allocate space for new matrix (non-root processes)
    if (rank != 0)
        img = img_make(width, height, pixelSize);
    else
        img = inImg;

    return img;
}

/**
 * Send an image part (data) to a process.
 * @param struct image_t *img The input image.
 * @param int offsetRowIdx The offset (as row index).
 * @param int limit The limit (amount of rows).
 * @param int destRank The destination rank.
 * @param int tag The message tag to use.
 */
void comm_sendImgPart(struct image_t *img, int offsetRowIdx, int limit,
    int destRank, int tag)
{
    // Checks
    if (offsetRowIdx >= img->height)
        return;
    if (limit + offsetRowIdx >= img->height)
        limit = img->height - offsetRowIdx;

    // Send
    MPI_Send(
        &(img->data[offsetRowIdx * img->pixelSize * img->width]),
        limit * img->pixelSize * img->width,
        MPI_CHAR,
        destRank, tag, MY_COMM
    );
}

/**
 * Receive an image part (data) to a process.
 * @param struct image_t *img The output image.
 * @param int offsetRowIdx The offset (as row index).
 * @param int limit The limit (amount of rows).
 * @param int sourceRank The source rank.
 * @param int tag The message tag to match.
 */
void comm_recvImgPart(struct image_t *img, int offsetRowIdx, int limit,
    int srcRank, int tag)
{
    MPI_Status st;

    // Checks
    if (offsetRowIdx >= img->height)
        return;
    if (limit + offsetRowIdx >= img->height)
        limit = img->height - offsetRowIdx;

    // Wildcards
    if (srcRank == COMM_ANY_RANK)
        srcRank = MPI_ANY_SOURCE;
    if (tag == COMM_ANY_TAG)
        tag = MPI_ANY_TAG;

    // Receive
    MPI_Recv(
        &(img->data[offsetRowIdx * img->pixelSize * img->width]),
        limit * img->pixelSize * img->width,
        MPI_CHAR,
        srcRank, tag, MY_COMM,
        &st    // status
    );
}
