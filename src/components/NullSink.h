#ifndef NULLSINK_H
#define NULLSINK_H

#include "src/core/Component.h"

class NullSink : public Component
{
public:
    NullSink();

    static const int InputPort1 = 240;
    static const int InputPort2 = 241;
    static const int InputPort3 = 242;
};

#endif // NULLSINK_H
