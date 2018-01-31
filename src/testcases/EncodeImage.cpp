#include "EncodeImage.h"

#include <fstream>

#include "src/core/CommonFunctions.h"
#include "src/core/Logger.h"
#include "src/components/EncoderJPEG.h"
#include "src/threadworkers/FileReader.h"
#include "src/threadworkers/FileWriter.h"

#define INPUT_FILENAME "test.image.yuv"
#define OUTPUT_FILENAME "test.omx.encoded.jpeg"

using namespace std;

class EncodeImage::DataClass
{
public:
    DataClass();
    ~DataClass();

    EncoderJPEG* encoderComponent;

    ifstream inputFile;
    ofstream outputFile;
    long remainingFileSize;
    bool inputBuffersCreated;
    bool outputBuffersCreated;
};

EncodeImage::DataClass::DataClass()
{
    encoderComponent = new EncoderJPEG();
    inputBuffersCreated = false;
    outputBuffersCreated = false;
    remainingFileSize = 0;
}

EncodeImage::DataClass::~DataClass()
{
    delete encoderComponent;
    encoderComponent = NULL;
}


EncodeImage::EncodeImage()
    : TestCase( TESTCASE_NAME_ENCODE_IMAGE )
{
    d = new DataClass();
}

EncodeImage::~EncodeImage()
{
    delete d;
}

void EncodeImage::Init()
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

void EncodeImage::Run()
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

    ok = d->encoderComponent->SetOutputImageParameters();
    if ( ok == false ) {
        LOG_ERR( "Error setting encoder output params" );
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

void EncodeImage::Destroy()
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
        bool ok = d->encoderComponent->DisablePortBuffers( EncoderJPEG::InputPort );
        if ( ok == false ) {
            LOG_ERR( "DisablePortBuffers failed" );
        } else {
            LOG_INFO( "DisablePortBuffers successful" );
        }
    }

    if ( d->outputBuffersCreated == true ) {
        bool ok = d->encoderComponent->DisablePortBuffers( EncoderJPEG::OutputPort );
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
