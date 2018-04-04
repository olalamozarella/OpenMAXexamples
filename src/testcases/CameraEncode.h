#ifndef CAMERAENCODE_H
#define CAMERAENCODE_H

#include <string>

class CameraEncode
{
public:
    CameraEncode();
    virtual ~CameraEncode();

    bool Init( std::string outputFileName );
    void Run( const long duration );
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // CAMERAENCODE_H
