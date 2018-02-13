#include "Tunnel.h"

#include "Component.h"
#include "CommonFunctions.h"
#include "Logger.h"

Tunnel::Tunnel( Component* source, OMX_U32 sourcePort, Component* destination, OMX_U32 destinationPort )
{
    this->source = source;
    this->destination = destination;
    this->sourcePort = sourcePort;
    this->destinationPort = destinationPort;
}

Tunnel::~Tunnel()
{
}

bool Tunnel::SetupTunnel()
{
//    OMX_STATETYPE sourceState;
//    bool ok = source->GetState( sourceState );
//    if ( ok == false ) {
//        LOG_ERR( "error getting source component state" );
//        return false;
//    }

//    if ( sourceState == OMX_StateLoaded ) {
//        LOG_ERR( "Wrong component state" );
//        return false;
//    }

    OMX_ERRORTYPE err = OMX_SetupTunnel( source->GetHandle(), sourcePort, destination->GetHandle(), destinationPort );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "Error OMX_SetupTunnel" + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    LOG_INFO( "Tunnel setup successful" );
    return true;
}
