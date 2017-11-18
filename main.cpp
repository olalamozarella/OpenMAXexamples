//cpp includes

//RPi includes
#include "bcm_host.h"

//project includes
#include "src/testcases/SystemInfo.h"
#include "src/testcases/ListComponents.h"
#include "src/testcases/PortInfo.h"
#include "src/testcases/NoBuffers.h"
#include "src/testcases/BuffersNoIL.h"

using namespace std;

int main()
{
    bcm_host_init();

//    SystemInfo systemInfo;
//    systemInfo.Run();

//    ListComponents testCase;
//    testCase.Run();

//    PortInfo portInfo;
//    portInfo.Run();

//    NoBuffers noBuffers;
//    noBuffers.Run();

    BuffersNoIL buffersNoIL;
    buffersNoIL.Init();
    buffersNoIL.Run();
    buffersNoIL.Destroy();

    bcm_host_deinit();

    return 0;
}


