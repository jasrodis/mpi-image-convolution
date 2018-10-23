/******************************************************************************
 * NAME:
 *  serial_main.c
 * DESCRIPTION:
 *  Image convolution main function.
 *****************************************************************************/
#include <stdio.h>
#include <util/log.h>
#include <types/image.h>
#include <types/matrix.h>
#include "cmd.h"
#include "convolution.h"
#include "timer.h"

/******************************************************************************
 * Data
 *****************************************************************************/

// Command line request
static CmdRequest *req;
// Input image
static struct image_t *inImg;
// Filter (matrix)
static struct matrix_t *filter;

/******************************************************************************
 * Helpers
 *****************************************************************************/

static void clean()
{
    log_log(LOG_DEBUG, "[CLEANING] Cleaning the memory...");
    if (filter != NULL) mat_destroy(filter);
    if (inImg != NULL) img_destroy(inImg);
    if (req != NULL) cmd_destroyRequest(req);
}

/******************************************************************************
 * Steps
 *****************************************************************************/

static int parseCmdRequest(int argc, char **argv)
{
    req = cmd_createRequest();
    if (cmd_parseRequest(argc, argv, req) == 0) {
        log_log(LOG_ERROR, "[CMD] Failed to parse command line!");
        clean();
        return 0;
    }
    if (req->verbose)
        log_setLogLevel(LOG_DEBUG);

    return 1;
}

static int parseFiles()
{
    // Parse input image
    inImg = img_makeFromFile(
        req->inputFile, req->imgWidth, req->imgHeight, req->imgPixelSize
    );
    if (inImg == NULL) return 0;
    log_log(LOG_DEBUG, "[PARSING] Image was parsed with:");
    log_log(LOG_DEBUG, "\twidth: %dpx,", req->imgWidth);
    log_log(LOG_DEBUG, "\theight: %dpx", req->imgHeight);
    log_log(LOG_DEBUG, "\tand %d bytes per pixel.", req->imgPixelSize);

    // Parse matrix (filter)
    filter = mat_makeFromFile(req->matrixFile);
    if (filter == NULL) return 0;
    log_log(LOG_DEBUG, "Filter matrix is %dx%d.", filter->width,
        filter->height);

    return 1;
}

/******************************************************************************
 * Main function
 *****************************************************************************/

int main(int argc, char **argv)
{
    struct image_t *outImg;
    double dist;
    int loops;
    double sTime, eTime;

    // Parsing command line
    if (!parseCmdRequest(argc, argv)) return 1;

    // Parse files
    if (!parseFiles()) return 2;

    // Start timer
    GET_TIME(sTime);

    // Process image
    outImg = NULL;
    loops = 0;
    do {
        // Run convolution
        log_log(LOG_DEBUG, "[RUNNING] Running convolution...");
        outImg = conv_run(inImg, filter);
        loops++;

        // Get dissimilarity
        dist = img_getDistance(inImg, outImg);
        log_log(LOG_DEBUG, "[RUNNING] New distance is %lf.", dist);

        // Remove previous in image
        img_destroy(inImg);
        inImg = outImg;
    } while (dist > 1.0 && loops < 1);

    // End timer
    GET_TIME(eTime);
    log_log(LOG_INFO, "The process took %lf seconds!", (eTime - sTime));

    // Write output image
    img_writeToFile(outImg, req->outputFile);

    // Clean up and exit
    clean();

    return 0;
}
