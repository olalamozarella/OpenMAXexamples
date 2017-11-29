#ifndef FILEREADER_H
#define FILEREADER_H

#include <fstream>
#include "src/core/ThreadWorker.h"
#include "IL/OMX_Core.h"

class Component;
class FileReader : public ThreadWorker
{
public:
    FileReader( Component* component, std::ifstream* inputFile, OMX_U32 port );
    ~FileReader();

    virtual bool DoSomething();

private:
    class DataClass;
    DataClass* d;
};

#endif // FILEREADER_H
