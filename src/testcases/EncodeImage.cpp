#include "EncodeImage.h"

#include <fstream>
#include "bcm_host.h"

#include "src/core/ProjectDefines.h"
#include "src/core/CommonFunctions.h"
#include "src/core/Logger.h"
#include "src/components/EncoderJPEG.h"
#include "src/threadworkers/FileReader.h"
#include "src/threadworkers/FileWriter.h"

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
{
    d = new DataClass();
}

EncodeImage::~EncodeImage()
{
    delete d;
}

bool EncodeImage::Init( string inputFileName, string outputFileName )
{
    d->inputFile.open( inputFileName, ios::in | ios::binary );
    if ( ( d->inputFile.good() == false ) || ( d->inputFile.is_open() == false ) ) {
        LOG_ERR( "Cannot open input file" );
        return false;
    }

    d->inputFile.seekg( 0, ios::end );
    d->remainingFileSize = d->inputFile.tellg();
    d->inputFile.seekg( 0, ios::beg );
    LOG_INFO( "Input file opened, size: " + INT2STR( d->remainingFileSize ) );

    d->outputFile.open( outputFileName, ios::out | ios::binary );
    if ( d->outputFile.is_open() == false ) {
        LOG_ERR( "Cannot open output file" );
        return false;
    }

    OMX_ERRORTYPE err = OMX_Init();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_Init failed" + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    LOG_INFO( "OMX_Init successful" );
    return true;
}

void EncodeImage::Run()
{
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

int main( int argc, char* argv[] )
{
    if ( argc < 3 ) {
        cout << "Too few parameters! Usage: ./EncodeImage <input file> <output file>" << endl;
        return -1;
    }

    struct timespec start, finish;
    LOG_INFO( "Starting testcase" );
    clock_gettime( CLOCK_MONOTONIC, &start );
    bcm_host_init();

    EncodeImage testcase;
    bool ok = testcase.Init( argv[1], argv[2] );
    if ( ok == false ) {
        return -1;
    }
    testcase.Run();
    testcase.Destroy();

    bcm_host_deinit();
    clock_gettime( CLOCK_MONOTONIC, &finish );

    double elapsed = finish.tv_sec - start.tv_sec;
    elapsed += ( finish.tv_nsec - start.tv_nsec ) / 1000000000.0;
    LOG_INFO( "Testcase finished, total time: " + FLOAT2STR( elapsed ) );
}
