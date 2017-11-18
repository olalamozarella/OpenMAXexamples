#include "Logger.h"

using namespace std;

void Logger::Info( std::string message )
{
    cout << "INFO: " <<message << endl;
}

void Logger::Warning( std::string message )
{
    cout << "WARNING: " << message << endl;
}

void Logger::Error( std::string message )
{
    cout << "ERROR: " << message << endl;
}

Logger::Logger()
{
    //private constructor
}

