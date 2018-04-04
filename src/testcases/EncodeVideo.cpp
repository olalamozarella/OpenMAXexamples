#include "EncodeVideo.h"

#include <fstream>
#include "bcm_host.h"

#include "src/core/ProjectDefines.h"
#include "src/core/CommonFunctions.h"
#include "src/core/Logger.h"
#include "src/components/EncoderH264.h"
#include "src/threadworkers/FileReader.h"
#include "src/threadworkers/FileWriter.h"

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
{
    d = new DataClass();
}

EncodeVideo::~EncodeVideo()
{
    delete d;
}

bool EncodeVideo::Init( string inputFileName, string outputFileName )
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

void EncodeVideo::Run()
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

    d->encoderComponent->GetVideoParameters( d->encoderComponent->OutputPort );

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

    ok = d->encoderComponent->ChangeState( OMX_StateExecuting );
    if ( ok == false ) {
        LOG_ERR( "Error changing state to executing" );
        return;
    }

    //feed some data
    bool portSettingChangedOccured = false;
    bool foundEOF = false;
    while ( portSettingChangedOccured == false ) {
        OMX_BUFFERHEADERTYPE* buffer;
        ok = d->encoderComponent->WaitForInputBuffer( d->encoderComponent->InputPort, buffer );
        if ( ( ok == false ) || ( buffer == NULL ) ) {
            LOG_ERR( "Error get input buffer" );
            break;
        }

        ok = CommonFunctions::ReadFileToBuffer( d->inputFile, buffer, foundEOF );
        if ( ok == false ) {
            // If reading fails, buffer is still empty and should be returned to component port-buffer collection.
            LOG_ERR( "read file failed - adding buffer back to map" );
            ok = d->encoderComponent->AddAllocatedBufferToMap( d->encoderComponent->InputPort, buffer );
            if ( ok == false ) {
                LOG_ERR( "Cannot add allocated buffer to map manually" );
            }
            break;
        }

        ok = d->encoderComponent->EmptyThisBuffer( buffer );
        if ( ok == false ) {
            // If reading fails, buffer is still empty and should be returned to component port-buffer collection.
            LOG_ERR( "empty first buffer failed, adding buffer back to map" );
            ok = d->encoderComponent->AddAllocatedBufferToMap( d->encoderComponent->InputPort, buffer );
            if ( ok == false ) {
                LOG_ERR( "Cannot add allocated buffer to map manually" );
            }
            break;
        }

        portSettingChangedOccured = d->encoderComponent->WaitForEvent( OMX_EventPortSettingsChanged, d->encoderComponent->OutputPort, 0, EVENT_HANDLER_TIMEOUT_MS_EXTENDED );
    }

    ok = d->encoderComponent->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing state to executing" );
        return;
    }

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

    d->encoderComponent->GetVideoParameters( d->encoderComponent->OutputPort );

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

int main( int argc, char* argv[] )
{
    if ( argc > 1 ) {
        string arg = argv[1];
        if ( arg == "-h" ) {
            cout << "This application encodes raw video data into H264 video. Input and output file paths should be passed as parameters." << endl;
            cout << "Usage: ./EncodeVideo <input file> <output file>" << endl;
            return 0;
        }
    }

    if ( argc < 3 ) {
        cout << "Too few parameters! Usage: ./EncodeVideo <input file> <output file>" << endl;
        return -1;
    }

    struct timespec start, finish;
    LOG_INFO( "Starting testcase" );
    clock_gettime( CLOCK_MONOTONIC, &start );
    bcm_host_init();

    EncodeVideo testcase;
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
