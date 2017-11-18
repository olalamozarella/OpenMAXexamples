#ifndef EVENT_H
#define EVENT_H

#include "IL/OMX_Component.h"

class Event
{
public:
    OMX_EVENTTYPE eventType;
    OMX_U32 data1;
    OMX_U32 data2;
    OMX_PTR eventData;
};

class FillBufferDone
{
public:
    OMX_BUFFERHEADERTYPE* buffer;
};

class EmptyBufferDone
{
public:
    OMX_BUFFERHEADERTYPE* buffer;
};
#endif // EVENT_H
