/******************************************************************************
 * NAME:
 *  util/log.c
 * DESCRIPTION:
 *  Logging implementation.
 *****************************************************************************/
#include <stdarg.h>
#include "log.h"

/******************************************************************************
 * Global variables
 *****************************************************************************/

static FILE *stream;
static LogLevel logLevel = LOG_INFO;
static char *levelName[4] = {"DEBUG", "INFO", "WARNING", "ERROR"};

/******************************************************************************
 * Log level management
 *****************************************************************************/

void log_setLogLevel(LogLevel newLogLevel)
{
    logLevel = newLogLevel;
}

/******************************************************************************
 * Log stream
 *****************************************************************************/

void log_setLogStream(FILE *newStream)
{
    stream = newStream;
}

/******************************************************************************
 * Log events
 *****************************************************************************/

void log_log(LogLevel level, const char *msg, ...)
{
    va_list argptr;
    char compiledMsg[1024];

    // Should log?
    if (level < logLevel)
        return;

    // Compile the message
    va_start(argptr, msg);
    vsprintf(compiledMsg, msg, argptr);
    va_end(argptr);

    // Write to log stream
    if (stream == NULL && level >= LOG_WARNING)
        fprintf(stderr, "[%s] %s\n", levelName[level], compiledMsg);
    else if (stream == NULL)
        fprintf(stdout, "[%s] %s\n", levelName[level], compiledMsg);
    else
        fprintf(stream, "[%s] %s\n", levelName[level], compiledMsg);
}
