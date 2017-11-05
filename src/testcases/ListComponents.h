#ifndef LISTCOMPONENTS_H
#define LISTCOMPONENTS_H

#include "TestCase.h"

class ListComponents : public TestCase
{
public:
    ListComponents();
    ~ListComponents();

    void Run();

private:
    void ListRoles( char* componentName );
};

#endif // LISTCOMPONENTS_H
