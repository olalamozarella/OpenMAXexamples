#ifndef ENCODEIMAGE_H
#define ENCODEIMAGE_H

#include <string>

class EncodeImage
{
public:
    EncodeImage();
    virtual ~EncodeImage();

    bool Init( std::string inputFileName, std::string outputFileName );
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // ENCODEIMAGE_H
