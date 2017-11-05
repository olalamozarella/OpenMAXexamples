#ifndef TESTCASE_H
#define TESTCASE_H

#include <string>
#include "src/ProjectDefines.h"

using namespace std;

class TestCase
{
public:
    TestCase( const string& caseName );
    virtual ~TestCase();

    virtual void Run();

private:
    string mTestCaseName;
};

#endif // TESTCASE_H
