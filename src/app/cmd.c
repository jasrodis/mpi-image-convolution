/******************************************************************************
 * NAME:
 *  cmd.c
 * DESCRIPTION:
 *  Command line parser implementation.
 *****************************************************************************/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "cmd.h"
#include <util/log.h>

/******************************************************************************
 * Internals
 *****************************************************************************/

static void showHelp()
{
    printf("Image convolution implementation\n");
    printf("  -d <Input image file path>\n");
    printf("  -o <Output image file path>\n");
    printf("  -m <Filter matrix file path>\n");
    printf("  -y <Image height>\n");
    printf("  -x <Image width>\n");
    printf("  -s <Image pixel size. Optional, default: 1>\n");
    printf("  -v Increases console output verbosity\n");
    printf("  -h Prints this help message\n");
}

static void handleErrorArgument()
{
    if (optopt == 'd' || optopt == 'm' || optopt == 'o' || optopt == 's'
        || optopt == 'h' || optopt == 'w') {
        log_log(LOG_ERROR, "[CMD] Option -%c requires an argument.", optopt);
    } else if (isprint(optopt)) {
        log_log(LOG_ERROR, "[CMD] Unknown option `-%c'.", optopt);
    } else {
        log_log(LOG_ERROR, "[CMD] Unknown option character `\\x%x'.", optopt);
    }
}

/******************************************************************************
 * Object creation/destruction
 *****************************************************************************/

/**
 * Creates a request.
 * @return CmdRequest* A request instance.
 */
CmdRequest *cmd_createRequest()
{
    CmdRequest *retVal = malloc(sizeof(CmdRequest));
    retVal->outputFile = stdout;
    retVal->inputFile = NULL;
    retVal->matrixFile = NULL;
    retVal->verbose = 0;
    retVal->imgHeight = 0;
    retVal->imgWidth = 0;
    retVal->imgPixelSize = 1;
    return retVal;
}

/**
 * Wipes out space allocated for a given request.
 * @param CmdRequest *req The request to wipe out.
 */
void cmd_destroyRequest(CmdRequest *req)
{
    if (req->inputFile != NULL) fclose(req->inputFile);
    if (req->matrixFile != NULL) fclose(req->matrixFile);
    if (req->outputFile != NULL && req->outputFile != stdout)
        fclose(req->outputFile);
    free(req);
}

/******************************************************************************
 * Request parsing
 *****************************************************************************/

/**
 * Parses the command line arguments and stores the information in a
 *  `CmdRequest` instance.
 * @param int argc The number of command line arguments.
 * @param char **argv A list of strings (char*) that are the command line
 *  arguments.
 * @param CmdRequest *req The filled command line request.
 * @return int 1 on success and 0 in case of failure.
 */
int cmd_parseRequest(int argc, char **argv, CmdRequest *req)
{
    char c;

    // Parse arguments
    while ((c = getopt(argc, argv, "vhd:m:o:s:x:y:")) != -1) {
        switch (c) {
            case 'h':  // Help
                showHelp();
                return 0;

            case 'v':  // Enable verbosity
                req->verbose = 1;
                break;

            case 'y':  // Image height
                sscanf(optarg, "%d", &(req->imgHeight));
                break;

            case 'x':  // Image width
                sscanf(optarg, "%d", &(req->imgWidth));
                break;

            case 's':  // Image pixel size
                sscanf(optarg, "%d", &(req->imgPixelSize));
                break;

            case 'd': // Input file path
                req->inputFile = fopen(optarg, "r");
                if (req->inputFile == NULL) {
                    log_log(LOG_ERROR, "[CMD] Failed to open %s: %s", optarg,
                        strerror(errno));
                    return 0;
                }
                break;

            case 'm': // Matrix file path
                req->matrixFile = fopen(optarg, "r");
                if (req->matrixFile == NULL) {
                    log_log(LOG_ERROR, "[CMD] Failed to open %s: %s", optarg,
                        strerror(errno));
                    return 0;
                }
                break;

            case 'o': // Output file path
                req->outputFile = fopen(optarg, "w");
                if (req->outputFile == NULL) {
                    log_log(LOG_ERROR, "[CMD] Failed to open %s: %s", optarg,
                        strerror(errno));
                    return 0;
                }
                break;

            case '?': // Unknown
                handleErrorArgument();
                return 0;

            default: // Unexpected!
                return 0;
        }
    }

    return 1;
}
