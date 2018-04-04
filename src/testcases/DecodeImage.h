#ifndef DECODEIMAGE_H
#define DECODEIMAGE_H

#include <string>

/**
 * @brief class DecodeImage - decodes image to raw image data. Input file format has to be decode-able by decoder component.
 */
class DecodeImage
{
public:
    DecodeImage();
    virtual ~DecodeImage();

    bool Init( std::string inputFileName, std::string outputFileName );
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // DECODEIMAGE_H
