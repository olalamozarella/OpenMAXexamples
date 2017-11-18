#include "BuffersNoIL.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include "src/core/CommonFunctions.h"

#include "src/core/Component.h"
#include "src/core/Logger.h"

#include "src/components/DecoderH264.h"

#include "ilclient.h"

using namespace std;

#define FILENAME "test.h264"

class BuffersNoIL::DataClass
{
public:
    Component* decoderComponent;

    DataClass()
    {
        decoderComponent = NULL;
    }

    ~DataClass()
    {
        delete decoderComponent;
        decoderComponent = NULL;
    }

    void EnableDecoderBuffers();
};

BuffersNoIL::BuffersNoIL()
    : TestCase( TESTCASE_NAME_BUFFERS_NOIL )
{
    d = new DataClass();
    d->decoderComponent = new DecoderH264();
}

BuffersNoIL::~BuffersNoIL()
{
    delete d;
}

void BuffersNoIL::Run()
{
    TestCase::Run();

//    FILE* inputFile = fopen( FILENAME, "rb" );
//    if ( inputFile == NULL ) {
//        cout << "Cannot open input file" << endl;
//        return;
//    }

    bool ok = d->decoderComponent->Init();
    if ( ok == false ) {
        LOG_ERR( "Error init component, destroying.." );
        return;
    }

    LOG_INFO( "Actual state: " + d->decoderComponent->GetComponentState() );

    ok = d->decoderComponent->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing state, destroying.." );
        return;
    }

    LOG_INFO( "Actual state: " + d->decoderComponent->GetComponentState() );

    ok = d->decoderComponent->EnablePortBuffers( 130 );
    if ( ok == false ) {
        LOG_ERR( "Error enable port buffers, destroying.." );
        return;
    }

    ok = d->decoderComponent->ChangeState( OMX_StateExecuting );
    if ( ok == false ) {
        LOG_ERR( "Error changing state, destroying.." );
        return;
    }
    LOG_INFO( "Actual state: " + d->decoderComponent->GetComponentState() );

    ok = d->decoderComponent->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing state, destroying.." );
        return;
    }
    LOG_INFO( "Actual state: " + d->decoderComponent->GetComponentState() );

    ok = d->decoderComponent->DisablePortBuffers( 130 );
    if ( ok == false ) {
        LOG_ERR( "Error disable port buffers, destroying.." );
        return;
    }
}

void BuffersNoIL::Init()
{
    OMX_ERRORTYPE err = OMX_Init();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_Init failed" + CommonFunctions::ErrorToString( err ) );
        return;
    }
    LOG_INFO( "OMX_Init successful" );
}

void BuffersNoIL::Destroy()
{
    OMX_ERRORTYPE err = OMX_Deinit();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_Deinit failed" + CommonFunctions::ErrorToString( err ) );
        return;
    }
    LOG_INFO( "OMX_Deinit successful" );
}
