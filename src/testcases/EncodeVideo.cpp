#include "EncodeVideo.h"

#include "src/components/EncoderH264.h"

class EncodeVideo::DataClass
{
public:
    DataClass();
    ~DataClass();

    EncoderH264* encoderComponent;
};

EncodeVideo::DataClass::DataClass()
{
    encoderComponent = new EncoderH264();
}

EncodeVideo::DataClass::~DataClass()
{
    delete encoderComponent;
    encoderComponent = NULL;
}


EncodeVideo::EncodeVideo()
    : TestCase( TESTCASE_NAME_ENCODE_VIDEO )
{
    d = new DataClass();
}

EncodeVideo::~EncodeVideo()
{
    delete d;
}

void EncodeVideo::Init()
{

}

void EncodeVideo::Run()
{
    TestCase::Run();
}

void EncodeVideo::Destroy()
{

}
