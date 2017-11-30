//cpp includes

//RPi includes
#include "bcm_host.h"

//project includes
#include "src/core/Logger.h"
#include "src/core/CommonFunctions.h"
#include "src/testcases/SystemInfo.h"
#include "src/testcases/ListComponents.h"
#include "src/testcases/PortInfo.h"
#include "src/testcases/NoBuffers.h"
#include "src/testcases/BuffersNoIL.h"
#include "src/testcases/VideoDecodeRender.h"
#include "src/testcases/MPEG2Stuff.h"
#include "src/testcases/ImageDecodeRender.h"
#include "src/testcases/DecodeVideo.h"

#include "ilclient.h"

#define LOGGER_PRINT_INFO
#define LOGGER_PRINT_WARN

using namespace std;

int main()
{
    struct timespec start, finish;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);

    bcm_host_init();

//    SystemInfo systemInfo;
//    systemInfo.Run();

//    ListComponents testCase;
//    testCase.Run();

//    PortInfo portInfo;
//    portInfo.Run();

//    NoBuffers noBuffers;
//    noBuffers.Run();

//    BuffersNoIL buffersNoIL;
//    buffersNoIL.Init();
//    buffersNoIL.Run();
//    buffersNoIL.Destroy();

//    VideoDecodeRender testcase;
//    testcase.Init();
//    testcase.Run();
//    testcase.Destroy();

//    MPEG2Stuff testcase;
//    testcase.Init();
//    testcase.Run();
//    testcase.Destroy();

//    ImageDecodeRender testcase;
//    testcase.Init();
//    testcase.Run();
//    testcase.Destroy();

    DecodeVideo testcase;
    testcase.Init();
    testcase.Run();
    testcase.Destroy();

    LOG_INFO( "Main - Testcase finished" );
    bcm_host_deinit();
    LOG_INFO( "Main - BCM host deinited" );

    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    LOG_INFO_VERBOSE( "Total time: " + FLOAT2STR( elapsed ) );

    return 0;
}


