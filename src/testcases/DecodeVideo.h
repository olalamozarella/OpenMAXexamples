#ifndef DECODEVIDEO_H
#define DECODEVIDEO_H

class DecodeVideo
{
public:
    DecodeVideo();
    virtual ~DecodeVideo();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // DECODEVIDEO_H
