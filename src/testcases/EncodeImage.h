#ifndef ENCODEIMAGE_H
#define ENCODEIMAGE_H

class EncodeImage
{
public:
    EncodeImage();
    virtual ~EncodeImage();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // ENCODEIMAGE_H
