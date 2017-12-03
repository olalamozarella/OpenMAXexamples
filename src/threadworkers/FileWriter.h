#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <fstream>
#include "src/core/ThreadWorker.h"
#include "IL/OMX_Core.h"

class Component;
/**
 * @brief class FileWriter - keeps writing data from buffers to file, finishes when EOS if found
 */
class FileWriter : public ThreadWorker
{
public:
    FileWriter( Component* component, std::ofstream* outputFile, OMX_U32 port );
    virtual ~FileWriter();

    virtual bool DoSomething();

private:
    class DataClass;
    DataClass* d;
};

#endif // FILEWRITER_H
