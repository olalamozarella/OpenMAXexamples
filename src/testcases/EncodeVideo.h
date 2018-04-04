#ifndef ENCODEVIDEO_H
#define ENCODEVIDEO_H

#include <string>

class EncodeVideo
{
public:
    EncodeVideo();
    virtual ~EncodeVideo();

    bool Init( std::string inputFileName, std::string outputFileName );
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // ENCODEVIDEO_H
