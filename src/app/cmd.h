/******************************************************************************
 * NAME:
 *  cmd.h
 * DESCRIPTION:
 *  Command line parser header files.
 *****************************************************************************/
#ifndef _CMD
#define _CMD

/******************************************************************************
 * Data structures
 *****************************************************************************/

typedef struct {
    FILE *outputFile;
    FILE *inputFile;
    FILE *matrixFile;
    int verbose;
    int imgHeight;
    int imgWidth;
    int imgPixelSize;
} CmdRequest;

/******************************************************************************
 * Object creation/destruction
 *****************************************************************************/

/**
 * Creates a request.
 * @return CmdRequest* A request instance.
 */
CmdRequest *cmd_createRequest();

/**
 * Wipes out space allocated for a given request.
 * @param CmdRequest *req The request to wipe out.
 */
void cmd_destroyRequest(CmdRequest *req);

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
int cmd_parseRequest(int argc, char **argv, CmdRequest *retVal);

#endif
