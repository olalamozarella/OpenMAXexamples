#ifndef CAMERARENDER_H
#define CAMERARENDER_H

class CameraRender
{
public:
    CameraRender();
    virtual ~CameraRender();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // CAMERARENDER_H
