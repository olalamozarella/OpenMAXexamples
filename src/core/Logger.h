#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

#define LOG_INFO Logger::Info
#define LOG_WARN Logger::Warning
#define LOG_ERR  Logger::Error

class Logger
{
public:
    static void Info( std::string message );
    static void Warning( std::string message );
    static void Error( std::string message );
private:
    Logger();
};

#endif // LOGGER_H
