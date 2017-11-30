#include "Logger.h"

#include <stdio.h>
#include <sys/time.h>

using namespace std;

void Logger::Info( string message , string file, int lineNumber , bool verbose )
{
#ifdef LOGGER_PRINT_INFO
    PrintCurrentTime();
    if ( verbose == true ) {
        printf( "INFO: %s (%s:%d)\n", message.c_str(), file.c_str(), lineNumber );
    } else {
        printf( "INFO: %s\n", message.c_str() );
    }
#endif
}

void Logger::Warning( string message , string file, int lineNumber )
{
#ifdef LOGGER_PRINT_WARN
    PrintCurrentTime();
    printf(  "WARNING: %s (%s:%d)\n", message.c_str(), file.c_str(), lineNumber );
#endif
}

void Logger::Error( string message , string file, int lineNumber )
{
    PrintCurrentTime();
    printf( "ERROR: %s (%s:%d)\n", message.c_str(), file.c_str(), lineNumber );
}

Logger::Logger()
{
    //private constructor
}

void Logger::PrintCurrentTime()
{
    timeval now;
    gettimeofday( &now, NULL );
    int milli = now.tv_usec / 1000;

    char buffer [80];
    strftime( buffer, 80, "%H:%M:%S", localtime( &now.tv_sec ) );
    //strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&now.tv_sec));

    printf( buffer );
    printf( ":%d ", milli );
}

