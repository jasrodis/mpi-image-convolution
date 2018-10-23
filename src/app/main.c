/******************************************************************************
 * NAME:
 *  main.c
 * DESCRIPTION:
 *  Image convolution main function.
 *****************************************************************************/
#include <stdio.h>
#include <math.h>
#include <util/log.h>
#include <types/image.h>
#include <types/matrix.h>
#include "cmd.h"
#include "comm.h"
#include "convolution.h"

/******************************************************************************
 * Data
 *****************************************************************************/

// Command line request
static CmdRequest *req;
// Input image
static struct image_t *inImg;
// Output image
static struct image_t *outImg;
// Filter (matrix)
static struct matrix_t *filter;
// Normalized filter (matrix)
static struct matrix_t *normFilter;

/******************************************************************************
 * Helpers
 *****************************************************************************/

static int parseCmdRequest(int argc, char **argv)
{
    // Create request
    req = cmd_createRequest();

    // Parse request
    if (cmd_parseRequest(argc, argv, req) == 0)
        return 0;

    // Set verbosity level
    if (req->verbose)
        log_setLogLevel(LOG_DEBUG);

    return 1;
}

static void clean()
{
    log_log(LOG_DEBUG, "[CLEANING] Cleaning the memory...");
    if (filter != NULL) mat_destroy(filter);
    if (normFilter != NULL) mat_destroy(normFilter);
    if (inImg != NULL) img_destroy(inImg);
    if (outImg != NULL) img_destroy(outImg);
    if (req != NULL) cmd_destroyRequest(req);
}

/******************************************************************************
 * Root process code
 *****************************************************************************/

static int root_validateRequest()
{
    // Input files
    if (req->inputFile == NULL) {
        log_log(LOG_ERROR, "[CMD] Please provide an input image file path!");
        return 0;
    }
    if (req->matrixFile == NULL) {
        log_log(LOG_ERROR, "[CMD] Please provide a filter matrix file path!");
        return 0;
    }

    // Output file
    if (req->outputFile == NULL) {
        log_log(LOG_ERROR, "[CMD] Please provide an output image file path!");
        return 0;
    }

    // Image data
    if (req->imgHeight == 0) {
        log_log(LOG_ERROR, "[CMD] Please provide the image height!");
        return 0;
    }
    if (req->imgWidth == 0) {
        log_log(LOG_ERROR, "[CMD] Please provide the image width!");
        return 0;
    }

    return 1;
}

static int root_parseFiles()
{
    // Parse input image
    inImg = img_makeFromFile(
        req->inputFile, req->imgWidth, req->imgHeight, req->imgPixelSize
    );
    if (inImg == NULL)
        return 0;

    // Log image messages
    log_log(LOG_DEBUG, "[PARSING] Image was parsed with:");
    log_log(LOG_DEBUG, "\twidth: %dpx,", req->imgWidth);
    log_log(LOG_DEBUG, "\theight: %dpx", req->imgHeight);
    log_log(LOG_DEBUG, "\tand %d bytes per pixel.", req->imgPixelSize);

    // Parse matrix (filter)
    filter = mat_makeFromFile(req->matrixFile);
    if (filter == NULL)
        return 0;

    // Log matrix messages
    log_log(LOG_DEBUG, "Filter matrix is %dx%d.", filter->width,
        filter->height);

    return 1;
}

static void root_run(int argc, char **argv)
{
    int i, size;
    int filterOffset;
    int offsetRowIdx, limit;
    double sTime, eTime;

    // Parse command line
    if (!parseCmdRequest(argc, argv))
        return;
    if (!root_validateRequest()) {
        clean();
        return;
    }

    // Parse input image and filter matrix
    if (!root_parseFiles()) {
        clean();
        return;
    }
    filterOffset = (filter->height - 1) / 2;

    // Send the filter matrix and the empty image to workers
    comm_broadcastMatrix(filter);
    comm_broadcastEmptyImg(inImg);

    // Start timer
    sTime = comm_wTime();

    // Send image parts
    size = comm_getSize();
    limit = ceil(inImg->height / (double) (size - 1));
    offsetRowIdx = 0;
    for (i = 1; i < size; i++) {
        comm_sendImgPart(
            inImg,
            (offsetRowIdx >= filterOffset) ? offsetRowIdx - filterOffset : 0,
            (offsetRowIdx + limit + (2 * filterOffset) < inImg->height)
                ? limit + (2 * filterOffset)
                : (inImg->height - offsetRowIdx),
            i,  // rank
            0   // tag
        );
        offsetRowIdx += limit;
    }

    // Receive results
    outImg = img_make(inImg->width, inImg->height, inImg->pixelSize);
    offsetRowIdx = 0;
    for (i = 1; i < size; i++) {
        comm_recvImgPart(outImg, offsetRowIdx, limit, i, 1);
        offsetRowIdx += limit;
    }

    // End timer
    eTime = comm_wTime();
    log_log(LOG_INFO, "The process took %lf seconds!", (eTime - sTime));

    // Write image
    img_writeToFile(outImg, req->outputFile);

    // Clean
    clean();
}

/******************************************************************************
 * Worker process code
 *****************************************************************************/

static void worker_run(int rank, int argc, char **argv)
{
    int size;
    int filterOffset;
    int offsetRowIdx, limit;

    // Parse command line
    if (!parseCmdRequest(argc, argv))
        return;

    // Get the filter matrix and the empty image
    filter = comm_broadcastMatrix(NULL);
    inImg = comm_broadcastEmptyImg(NULL);
    filterOffset = (filter->height - 1) / 2;

    // Normalize filter
    normFilter = conv_normalizeFilter(filter);

    // Get image part
    size = comm_getSize();
    limit = ceil(inImg->height / (double) (size - 1));
    offsetRowIdx = limit * (rank - 1);
    comm_recvImgPart(
        inImg,
        (offsetRowIdx >= filterOffset) ? offsetRowIdx - filterOffset : 0,
        (offsetRowIdx + limit + (2 * filterOffset) < inImg->height)
            ? limit + (2 * filterOffset)
            : (inImg->height - offsetRowIdx),
        0,  // rank
        0   // tag
    );

    // Run convolution
    outImg = img_make(inImg->width, inImg->height, inImg->pixelSize);
    conv_runPartially(inImg, offsetRowIdx, limit, outImg, normFilter);

    // Send back results
    comm_sendImgPart(outImg, offsetRowIdx, limit, 0, 1);

    // Clean
    clean();
}

/******************************************************************************
 * Main function
 *****************************************************************************/

int main(int argc, char **argv)
{
    int rank;

    // Start communicator
    rank = comm_start(&argc, &argv);

    // SPMD branching...
    if (rank == 0)
        root_run(argc, argv);
    else
        worker_run(rank, argc, argv);

    // Stop communicator
    comm_stop();

    return 0;
}
