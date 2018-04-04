#ifndef CAMERARENDER_H
#define CAMERARENDER_H

class CameraRender
{
public:
    CameraRender();
    virtual ~CameraRender();

    bool Init();
    void Run( const long duration );
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // CAMERARENDER_H
