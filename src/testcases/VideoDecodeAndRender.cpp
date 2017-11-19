#include "VideoDecodeAndRender.h"

#include "src/components/DecoderH264.h"
#include "src/core/Logger.h"

class VideoDecodeAndRender::DataClass
{
public:
    Component* decoder;
    Component* renderer;

    DataClass()
    {
        decoder = NULL;
        renderer = NULL;
    }

    ~DataClass()
    {
        delete decoder;
        delete renderer;
    }
};

VideoDecodeAndRender::VideoDecodeAndRender()
    : TestCase( TESTCASE_NAME_VIDEO_DECDE_RENDER )
{
    d = new DataClass();
}

VideoDecodeAndRender::~VideoDecodeAndRender()
{
    delete d;
}

void VideoDecodeAndRender::Init()
{
    OMX_ERRORTYPE err = OMX_Init();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_Init failed" + CommonFunctions::ErrorToString( err ) );
        return;
    }
    LOG_INFO( "OMX_Init successful" );
}

void VideoDecodeAndRender::Run()
{
    TestCase::Run();

    FILE* inputFile = fopen( FILENAME, "rb" );
    if ( inputFile == NULL ) {
        cout << "Cannot open input file" << endl;
        return;
    }

    d->decoder->Init();
    d->renderer->Init();
}

void VideoDecodeAndRender::Destroy()
{
    OMX_ERRORTYPE err = OMX_Deinit();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_Deinit failed" + CommonFunctions::ErrorToString( err ) );
        return;
    }
    LOG_INFO( "OMX_Deinit successful" );
}
