#ifndef ENCODEVIDEO_H
#define ENCODEVIDEO_H

class EncodeVideo
{
public:
    EncodeVideo();
    virtual ~EncodeVideo();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // ENCODEVIDEO_H
