#include "EncodeImage.h"

#include "src/components/EncoderJPEG.h"

class EncodeImage::DataClass
{
public:
    DataClass();
    ~DataClass();

    EncoderJPEG* encoderComponent;
};

EncodeImage::DataClass::DataClass()
{
    encoderComponent = new EncoderJPEG();
}

EncodeImage::DataClass::~DataClass()
{
    delete encoderComponent;
    encoderComponent = NULL;
}


EncodeImage::EncodeImage()
    : TestCase( TESTCASE_NAME_ENCODE_VIDEO )
{
    d = new DataClass();
}

EncodeImage::~EncodeImage()
{
    delete d;
}

void EncodeImage::Init()
{

}

void EncodeImage::Run()
{
    TestCase::Run();
}

void EncodeImage::Destroy()
{

}
