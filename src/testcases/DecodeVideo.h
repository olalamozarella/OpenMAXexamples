#ifndef DECODEVIDEO_H
#define DECODEVIDEO_H

#include <string>

class DecodeVideo
{
public:
    DecodeVideo();
    virtual ~DecodeVideo();

    bool Init( std::string inputFileName, std::string outputFileName );
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // DECODEVIDEO_H
