//#include "DecodeVideo.h"

//#include <fstream>

//#include "src/core/CommonFunctions.h"
//#include "src/core/Logger.h"
//#include "src/core/Tunnel.h"
//#include "src/components/DecoderH264.h"
//#include "src/components/NullSink.h"
//#include "src/threadworkers/FileReader.h"
//#include "src/threadworkers/FileWriter.h"

//#define FILENAME "test.h264"
//#define OUTPUT_FILENAME "test.omx.yuv"

//using namespace std;

//class DecodeVideo::DataClass
//{
//public:
//    DataClass();
//    ~DataClass();

//    DecoderH264* decoder;
//    NullSink* nullSink;

//    ifstream inputFile;

//    long remainingFileSize;
//    bool inputBuffersCreated;
//};

//DecodeVideo::DataClass::DataClass()
//{
//    inputBuffersCreated = false;
//    remainingFileSize = 0;
//}

//DecodeVideo::DataClass::~DataClass()
//{
//}

//DecodeVideo::DecodeVideo()
//    : TestCase( TESTCASE_NAME_DECODE_VIDEO )
//{
//    d = new DataClass();
//    d->decoder = new DecoderH264();
//    d->nullSink = new NullSink();
//}

//DecodeVideo::~DecodeVideo()
//{
//    delete d->nullSink;
//    delete d->decoder;
//    delete d;
//}

//void DecodeVideo::Init()
//{
//    OMX_ERRORTYPE err = OMX_Init();
//    if ( err != OMX_ErrorNone ) {
//        LOG_ERR( "OMX_Init failed" + CommonFunctions::ErrorToString( err ) );
//        return;
//    }
//    LOG_INFO( "OMX_Init successful" );

//    d->inputFile.open( FILENAME, ios::in | ios::binary );
//    if ( d->inputFile.is_open() == false ) {
//        LOG_ERR( "Cannot open input file" );
//        return;
//    }

//    d->inputFile.seekg( 0, ios::end );
//    d->remainingFileSize = d->inputFile.tellg();
//    d->inputFile.seekg( 0, ios::beg );

//    LOG_INFO( "File opened, size: " + INT2STR( d->remainingFileSize ) );
//}

//void DecodeVideo::Run()
//{
//    TestCase::Run();

//    bool ok = d->decoder->Init();
//    if ( ok == false ) {
//        LOG_ERR( "Error init component, destroying.." );
//        return;
//    }

//    ok = d->decoder->ChangeState( OMX_StateIdle );
//    if ( ok == false ) {
//        LOG_ERR( "Error changing state, destroying.." );
//        return;
//    }

//    LOG_INFO( "Actual decoder state: " + d->decoder->GetComponentState() );

//    ok = d->nullSink->Init();
//    if ( ok == false ) {
//        LOG_ERR( "Error init component, destroying.." );
//        return;
//    }

//    ok = d->nullSink->ChangeState( OMX_StateIdle );
//    if ( ok == false ) {
//        LOG_ERR( "Error changing state, destroying.." );
//        return;
//    }

//    ok = d->decoder->SetVideoParameters();
//    if ( ok == false ) {
//        LOG_ERR( "Error setting decoder params" );
//        return;
//    }

//    ok = d->decoder->EnablePortBuffers( DecoderH264::InputPort );
//    if ( ok == false ) {
//        LOG_ERR( "Error enabling port buffers" );
//        return;
//    }
//    d->inputBuffersCreated = true;

//    ok = d->decoder->ChangeState( OMX_StateExecuting );
//    if ( ok == false ) {
//        LOG_ERR( "Error changing state to executing" );
//        return;
//    }

//    d->decoder->GetVideoParameters( d->decoder->OutputPort );

//    //feed some data
//    bool portSettingChangedOccured = false;
//    bool foundEOF = false;
//    while ( portSettingChangedOccured == false ) {
//        OMX_BUFFERHEADERTYPE* buffer;
//        ok = d->decoder->WaitForInputBuffer( DecoderH264::InputPort, buffer );
//        if ( ( ok == false ) || ( buffer == NULL ) ) {
//            LOG_ERR( "Error get input buffer" );
//            break;
//        }

//        ok = CommonFunctions::ReadFileToBuffer( d->inputFile, buffer, foundEOF );
//        if ( ok == false ) {
//            // If reading fails, buffer is still empty and should be returned to component port-buffer collection.
//            LOG_ERR( "read file failed - adding buffer back to map" );
//            ok = d->decoder->AddAllocatedBufferToMap( DecoderH264::InputPort, buffer );
//            if ( ok == false ) {
//                LOG_ERR( "Cannot add allocated buffer to map manually" );
//            }
//            break;
//        }

//        ok = d->decoder->EmptyThisBuffer( buffer );
//        if ( ok == false ) {
//            // If reading fails, buffer is still empty and should be returned to component port-buffer collection.
//            LOG_ERR( "empty first buffer failed, adding buffer back to map" );
//            ok = d->decoder->AddAllocatedBufferToMap( DecoderH264::InputPort, buffer );
//            if ( ok == false ) {
//                LOG_ERR( "Cannot add allocated buffer to map manually" );
//            }
//            break;
//        }

//        portSettingChangedOccured = d->decoder->WaitForEvent( OMX_EventPortSettingsChanged, DecoderH264::OutputPort, 0, EVENT_HANDLER_TIMEOUT_MS_EXTENDED );
//    }

//    d->decoder->GetVideoParameters( d->decoder->OutputPort );

//    ok = d->decoder->ChangeState( OMX_StateIdle );
//    if ( ok == false ) {
//        LOG_ERR( "Error changing state to executing" );
//        return;
//    }

//    Tunnel tunnelDecoderNullsink( d->decoder, DecoderH264::OutputPort, d->nullSink, d->nullSink->InputPort1 );
//    ok = tunnelDecoderNullsink.SetupTunnel();
//    if ( ok == false ) {
//        LOG_ERR( "Error setup tunnel" );
//        return;
//    }

//    ok = d->nullSink->EnablePort( d->nullSink->InputPort1 );
//    if ( ok == false ) {
//        LOG_ERR( "Error enabling nullsink input port" );
//        return;
//    }

//    ok = d->nullSink->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, d->nullSink->InputPort1, EVENT_HANDLER_TIMEOUT_MS_MAX );
//    if ( ok == false ) {
//        LOG_ERR( "Error enabling nullsink input port - event did not come during timeout" );
//        return;
//    }

//    ok = d->decoder->EnablePort( d->decoder->OutputPort );
//    if ( ok == false ) {
//        LOG_ERR( "Error enabling nullsink input port" );
//        return;
//    }

//    ok = d->decoder->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, d->decoder->OutputPort, EVENT_HANDLER_TIMEOUT_MS_MAX );
//    if ( ok == false ) {
//        LOG_ERR( "Error enabling nullsink input port - event did not come during timeout" );
//        return;
//    }

//    ok = d->nullSink->ChangeState( OMX_StateExecuting );
//    if ( ok == false ) {
//        LOG_ERR( "Error changing state to executing" );
//        return;
//    }

//    ok = d->decoder->ChangeState( OMX_StateExecuting );
//    if ( ok == false ) {
//        LOG_ERR( "Error changing state to executing" );
//        return;
//    }

//    FileReader fileReader( d->decoder, &d->inputFile, d->decoder->InputPort );
//    fileReader.Start();
//    fileReader.WaitForThreadJoin();

//    LOG_INFO( "Finished run" );
//}

//void DecodeVideo::Destroy()
//{
//    if ( d->inputFile.is_open() == true ) {
//        d->inputFile.close();
//        LOG_INFO( "input file closed" );
//    }

//    if ( d->inputBuffersCreated == true ) {
//        bool ok = d->decoder->DisablePortBuffers( DecoderH264::InputPort );
//        if ( ok == false ) {
//            LOG_ERR( "DisablePortBuffers failed" );
//        } else {
//            LOG_INFO( "DisablePortBuffers successful" );
//        }
//    }

//    OMX_ERRORTYPE err = OMX_Deinit();
//    if ( err != OMX_ErrorNone ) {
//        LOG_ERR( "OMX_Deinit failed" + CommonFunctions::ErrorToString( err ) );
//        return;
//    }

//    LOG_INFO( "OMX_Deinit successful" );
//}
