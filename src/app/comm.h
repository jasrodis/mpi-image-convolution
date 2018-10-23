/******************************************************************************
 * NAME:
 *  comm.h
 * DESCRIPTION:
 *  Communications module header file.
 *****************************************************************************/
#ifndef _COMM
#define _COMM

#include <types/matrix.h>
#include <types/image.h>

/******************************************************************************
 * Constants
 *****************************************************************************/

#define COMM_ANY_RANK -1
#define COMM_ANY_TAG -1

/******************************************************************************
 * Start / stop the communication
 *****************************************************************************/

/**
 * Starts the communication session.
 * @param int *argc The arguments count (pointer).
 * @param char ***argv The list of arguments (pointer).
 * @return int The rank (id) of the process.
 */
int comm_start(int *argc, char ***argv);

/**
 * Stop the communication session.
 */
void comm_stop();

/******************************************************************************
 * Timing
 *****************************************************************************/

/**
 * Returns the wall clock time in a parallelism-safe way.
 * @return double The wall clock time.
 */
double comm_wTime();

/******************************************************************************
 * Information
 *****************************************************************************/

/**
 * Returns the size of the pool.
 * @return int The size of the pool.
 */
int comm_getSize();

/**
 * Returns the rank of the process.
 * @return int The rank of the process.
 */
int comm_getRank();

/******************************************************************************
 * Matrix transferring
 *****************************************************************************/

/**
 * Broadcast a matrix to all processes of the communicator.
 * @param struct matrix_t *inMat The matrix to broadcast. Set to NULL for all
 *  non-root processes.
 * @return struct matirx_t* The broadcasted matrix. NULL in case of failure.
 */
struct matrix_t* comm_broadcastMatrix(struct matrix_t *inMat);

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
struct image_t* comm_broadcastEmptyImg(struct image_t *inImg);

/**
 * Send an image part (data) to a process.
 * @param struct image_t *img The input image.
 * @param int offsetRowIdx The offset (as row index).
 * @param int limit The limit (amount of rows).
 * @param int destRank The destination rank.
 * @param int tag The message tag to use.
 */
void comm_sendImgPart(struct image_t *img, int offsetRowIdx, int limit,
    int destRank, int tag);

/**
 * Receive an image part (data) to a process.
 * @param struct image_t *img The output image.
 * @param int offsetRowIdx The offset (as row index).
 * @param int limit The limit (amount of rows).
 * @param int sourceRank The source rank.
 * @param int tag The message tag to match.
 */
void comm_recvImgPart(struct image_t *img, int offsetRowIdx, int limit,
    int srcRank, int tag);

#endif
