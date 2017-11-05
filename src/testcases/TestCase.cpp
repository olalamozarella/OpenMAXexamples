#include "TestCase.h"

#include <iostream>

TestCase::TestCase( const string& caseName )
{
    mTestCaseName = caseName;
}

TestCase::~TestCase()
{

}

void TestCase::Run()
{
    cout << "Starting test case: " << mTestCaseName << endl;
}
