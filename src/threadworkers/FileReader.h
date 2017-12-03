#ifndef FILEREADER_H
#define FILEREADER_H

#include <fstream>
#include "src/core/ThreadWorker.h"
#include "IL/OMX_Core.h"

class Component;

/**
 * @brief class FileReader - keeps reading file to input buffers of a component.
 *
 * Execution steps:
 *  - take buffer from component's input buffer collection
 *  - read chunk of file into buffer
 *    - set flag if EOF is found
 *  - call EmptyThisBuffer
 *  - if EOF was not found, repeat
 *  - if EOF was found, wait until all buffers are returned to the component input buffer collection
 */
class FileReader : public ThreadWorker
{
public:
    FileReader( Component* component, std::ifstream* inputFile, OMX_U32 port );
    virtual ~FileReader();

    virtual bool DoSomething();

private:
    class DataClass;
    DataClass* d;
};

#endif // FILEREADER_H
