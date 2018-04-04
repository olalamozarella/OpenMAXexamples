#include "DecodeImage.h"

#include <fstream>
#include "bcm_host.h"

#include "src/core/ProjectDefines.h"
#include "src/core/CommonFunctions.h"
#include "src/core/Logger.h"
#include "src/core/Tunnel.h"
#include "src/components/DecoderJPEG.h"
#include "src/threadworkers/FileReader.h"
#include "src/threadworkers/FileWriter.h"

using namespace std;

class DecodeImage::DataClass
{
public:
    DataClass();
    ~DataClass();

    DecoderJPEG* decoder;

    ifstream inputFile;
    ofstream outputFile;
    long remainingFileSize;
    bool inputBuffersCreated;
    bool outputBuffersCreated;
};

DecodeImage::DataClass::DataClass()
{
    inputBuffersCreated = false;
    outputBuffersCreated = false;
    remainingFileSize = 0;
}

DecodeImage::DataClass::~DataClass()
{
}

DecodeImage::DecodeImage()
{
    d = new DataClass();
    d->decoder = new DecoderJPEG();
}

DecodeImage::~DecodeImage()
{
    delete d->decoder;
    delete d;
}

bool DecodeImage::Init( string inputFileName, string outputFileName )
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

void DecodeImage::Run()
{
    bool ok = d->decoder->Init();
    if ( ok == false ) {
        LOG_ERR( "Error init component, destroying.." );
        return;
    }

    ok = d->decoder->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing state, destroying.." );
        return;
    }

    LOG_INFO( "Actual decoder state: " + d->decoder->GetComponentState() );

    ok = d->decoder->SetImageParameters();
    if ( ok == false ) {
        LOG_ERR( "Error setting decoder params" );
        return;
    }

    ok = d->decoder->EnablePortBuffers( DecoderJPEG::InputPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling port buffers" );
        return;
    }
    d->inputBuffersCreated = true;

    ok = d->decoder->ChangeState( OMX_StateExecuting );
    if ( ok == false ) {
        LOG_ERR( "Error changing state to executing" );
        return;
    }

    //feed some data - image_decode should need just single buffer for port setting changed
    bool portSettingChangedOccured = false;
    bool foundEOF = false;
    while ( portSettingChangedOccured == false ) {
        OMX_BUFFERHEADERTYPE* buffer;
        ok = d->decoder->WaitForInputBuffer( DecoderJPEG::InputPort, buffer );
        if ( ( ok == false ) || ( buffer == NULL ) ) {
            LOG_ERR( "Error get input buffer" );
            break;
        }

        ok = CommonFunctions::ReadFileToBuffer( d->inputFile, buffer, foundEOF );
        if ( ok == false ) {
            // If reading fails, buffer is still empty and should be returned to component port-buffer collection.
            LOG_ERR( "read file failed - adding buffer back to map" );
            ok = d->decoder->AddAllocatedBufferToMap( DecoderJPEG::InputPort, buffer );
            if ( ok == false ) {
                LOG_ERR( "Cannot add allocated buffer to map manually" );
            }
            break;
        }

        ok = d->decoder->EmptyThisBuffer( buffer );
        if ( ok == false ) {
            // If reading fails, buffer is still empty and should be returned to component port-buffer collection.
            LOG_ERR( "empty first buffer failed, adding buffer back to map" );
            ok = d->decoder->AddAllocatedBufferToMap( DecoderJPEG::InputPort, buffer );
            if ( ok == false ) {
                LOG_ERR( "Cannot add allocated buffer to map manually" );
            }
            break;
        }

        portSettingChangedOccured = d->decoder->WaitForEvent( OMX_EventPortSettingsChanged, DecoderJPEG::OutputPort, 0, EVENT_HANDLER_TIMEOUT_MS_EXTENDED );
    }

    ok = d->decoder->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing state to executing" );
        return;
    }

    ok = d->decoder->EnablePortBuffers( DecoderJPEG::OutputPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling output ports" );
        return;
    }
    d->outputBuffersCreated = true;

    ok = d->decoder->ChangeState( OMX_StateExecuting );
    if ( ok == false ) {
        LOG_ERR( "Error changing state to executing" );
        return;
    }

    FileWriter fileWriter( d->decoder, &d->outputFile, d->decoder->OutputPort );
    fileWriter.Start();

    FileReader fileReader( d->decoder, &d->inputFile, d->decoder->InputPort );
    fileReader.Start();

    fileReader.WaitForThreadJoin();
    fileWriter.WaitForThreadJoin();

    LOG_INFO( "Finished run" );
}

void DecodeImage::Destroy()
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
        bool ok = d->decoder->DisablePortBuffers( DecoderJPEG::InputPort );
        if ( ok == false ) {
            LOG_ERR( "DisablePortBuffers failed" );
        } else {
            LOG_INFO( "DisablePortBuffers successful" );
        }
    }

    if ( d->outputBuffersCreated == true ) {
        bool ok = d->decoder->DisablePortBuffers( DecoderJPEG::OutputPort );
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
        cout << "Too few parameters! Usage: ./DecodeImage <input file> <output file>" << endl;
        return -1;
    }

    struct timespec start, finish;
    LOG_INFO( "Starting testcase" );
    clock_gettime( CLOCK_MONOTONIC, &start );
    bcm_host_init();

    DecodeImage testcase;
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
