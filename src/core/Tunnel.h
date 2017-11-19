#ifndef TUNNEL_H
#define TUNNEL_H

#include "IL/OMX_Types.h"

class Component;
class Tunnel
{
public:
    Tunnel( Component* source, OMX_U32 sourcePort, Component* destination, OMX_U32 destinationPort );
    ~Tunnel();

    bool SetupTunnel();

private:
    Component* source;
    Component* destination;
    OMX_U32 sourcePort;
    OMX_U32 destinationPort;
};

#endif // TUNNEL_H
