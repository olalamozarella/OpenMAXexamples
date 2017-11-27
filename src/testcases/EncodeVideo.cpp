#include "EncodeVideo.h"

#include "src/core/Component.h"

class EncodeVideo::DataClass
{
public:
    DataClass();
    ~DataClass();

    Component* encoderComponent;
};

EncodeVideo::DataClass::DataClass()
{
    encoderComponent = new Component( "OMX.broadcom.video_encode" );
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
