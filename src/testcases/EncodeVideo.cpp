#include "EncodeVideo.h"

#include <fstream>

#include "src/core/CommonFunctions.h"
#include "src/core/Logger.h"
#include "src/components/EncoderH264.h"
#include "src/threadworkers/FileReader.h"
#include "src/threadworkers/FileWriter.h"

#define INPUT_FILENAME "test.video.raw"
#define OUTPUT_FILENAME "test.h264.encoded"

using namespace std;

class EncodeVideo::DataClass
{
public:
    DataClass();
    ~DataClass();

    EncoderH264* encoderComponent;

    ifstream inputFile;
    ofstream outputFile;
    long remainingFileSize;
    bool inputBuffersCreated;
    bool outputBuffersCreated;
};

EncodeVideo::DataClass::DataClass()
{
    encoderComponent = new EncoderH264();
    inputBuffersCreated = false;
    outputBuffersCreated = false;
    remainingFileSize = 0;
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
    OMX_ERRORTYPE err = OMX_Init();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_Init failed" + CommonFunctions::ErrorToString( err ) );
        return;
    }
    LOG_INFO( "OMX_Init successful" );

    d->inputFile.open( INPUT_FILENAME, ios::in | ios::binary );
    if ( d->inputFile.is_open() == false ) {
        LOG_ERR( "Cannot open input file" );
        return;
    }

    d->inputFile.seekg( 0, ios::end );
    d->remainingFileSize = d->inputFile.tellg();
    d->inputFile.seekg( 0, ios::beg );

    d->outputFile.open( OUTPUT_FILENAME, ios::out | ios::binary );
    if ( d->outputFile.is_open() == false ) {
        LOG_ERR( "Cannot open input file" );
        return;
    }

    LOG_INFO( "File opened, size: " + INT2STR( d->remainingFileSize ) );
}

void EncodeVideo::Run()
{
    TestCase::Run();

    bool ok = d->encoderComponent->Init();
    if ( ok == false ) {
        LOG_ERR( "Error init component, destroying.." );
        return;
    }

    ok = d->encoderComponent->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing state, destroying.." );
        return;
    }

    LOG_INFO( "Actual decoder state: " + d->encoderComponent->GetComponentState() );

    ok = d->encoderComponent->SetVideoParameters();
    if ( ok == false ) {
        LOG_ERR( "Error setting decoder params" );
        return;
    }

    ok = d->encoderComponent->EnablePortBuffers( d->encoderComponent->InputPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling port buffers" );
        return;
    }
    d->inputBuffersCreated = true;

    ok = d->encoderComponent->EnablePortBuffers( d->encoderComponent->OutputPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling output ports" );
        return;
    }
    d->outputBuffersCreated = true;

    ok = d->encoderComponent->ChangeState( OMX_StateExecuting );
    if ( ok == false ) {
        LOG_ERR( "Error changing state to executing" );
        return;
    }

    FileWriter fileWriter( d->encoderComponent, &d->outputFile, d->encoderComponent->OutputPort );
    fileWriter.Start();

    FileReader fileReader( d->encoderComponent, &d->inputFile, d->encoderComponent->InputPort );
    fileReader.Start();

    fileReader.WaitForThreadJoin();
    fileWriter.WaitForThreadJoin();

    LOG_INFO( "Finished run" );
}

void EncodeVideo::Destroy()
{
    if ( d->inputFile.is_open() == true ) {
        d->inputFile.close();
        LOG_INFO( "input file closed" );
    }

    if ( d->outputFile.is_open() == true ) {
        d->outputFile.close();
        LOG_INFO( "output file closed" );
    }

    if ( d->inputBuffersCreated == true ) {
        bool ok = d->encoderComponent->DisablePortBuffers( EncoderH264::InputPort );
        if ( ok == false ) {
            LOG_ERR( "DisablePortBuffers failed" );
        } else {
            LOG_INFO( "DisablePortBuffers successful" );
        }
    }

    if ( d->outputBuffersCreated == true ) {
        bool ok = d->encoderComponent->DisablePortBuffers( EncoderH264::OutputPort );
        if ( ok == false ) {
            LOG_ERR( "DisablePortBuffers failed" );
        } else {
            LOG_INFO( "DisablePortBuffers successful" );
        }
    }

    OMX_ERRORTYPE err = OMX_Deinit();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_Deinit failed" + CommonFunctions::ErrorToString( err ) );
        return;
    }

    LOG_INFO( "OMX_Deinit successful" );
}
