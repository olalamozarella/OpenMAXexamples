//cpp includes

//RPi includes
#include "bcm_host.h"

//project includes
#include "src/testcases/ListComponents.h"
#include "src/testcases/SystemInfo.h"

using namespace std;

int main()
{
    bcm_host_init();

//    SystemInfo systemInfo;
//    systemInfo.Run();

    ListComponents testCase;
    testCase.Run();

    bcm_host_deinit();

    return 0;
}


