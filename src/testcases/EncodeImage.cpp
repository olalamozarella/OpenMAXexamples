#include "EncodeImage.h"

#include "src/core/Component.h"

class EncodeImage::DataClass
{
public:
    DataClass();
    ~DataClass();

    Component* encoderComponent;
};

EncodeImage::DataClass::DataClass()
{
    encoderComponent = new Component( "OMX.broadcom.image_encode" );
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
