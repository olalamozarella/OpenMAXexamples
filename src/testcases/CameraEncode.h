#ifndef CAMERAENCODE_H
#define CAMERAENCODE_H

class CameraEncode
{
public:
    CameraEncode();
    virtual ~CameraEncode();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // CAMERAENCODE_H
