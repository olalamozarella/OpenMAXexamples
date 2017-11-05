#include "SystemInfo.h"

#include <iostream>
#include <stdlib.h>

SystemInfo::SystemInfo()
    : TestCase( TESTCASE_NAME_SYSTEM_INFO )
{

}

SystemInfo::~SystemInfo()
{

}

void SystemInfo::Run()
{
    TestCase::Run();

    cout << "Hello Worldrd!" << endl;
    system( "cat /proc/cpuinfo" );

    cout << endl;
}

