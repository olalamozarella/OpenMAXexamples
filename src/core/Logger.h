#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

#define LOG_INFO(x) Logger::Info(x, __FILE__, __LINE__, false )
#define LOG_INFO_VERBOSE(x) Logger::Info(x, __FILE__, __LINE__, true )
#define LOG_WARN(x) Logger::Warning(x, __FILE__, __LINE__)
#define LOG_ERR(x) Logger::Error(x, __FILE__, __LINE__)

class Logger
{
public:
    static void Info( std::string message, std::string file, int lineNumber, bool verbose );
    static void Warning( std::string message, std::string file, int lineNumber );
    static void Error( std::string message, std::string file, int lineNumber );
private:
    Logger();
    static void PrintCurrentTime();
};

#endif // LOGGER_H
