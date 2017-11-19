### Cmake file for simple RPi projects ###

#Reload cmake toolchain file with "Run CMake", parameters:
#cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=/opt/rpi-cross-compile-essentials/pi.cmake ./

#Add every class to HEADERS and SOURCES, otherwise it wont be displayed in QTCreator project.
#Add external libraries with TARGET_LINK_LIBRARIES()

cmake_minimum_required(VERSION 3.1)

project(cmake-test-qtcreator)

# Useful CMake options for Qt projects
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Compile flags
#set(CMAKE_BUILD_TYPE Debug)
set(COMPILE_DEFINITIONS -Werror)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRASPBERRYPI -DOMX_SKIP64BIT")

# List of all header files that belong to this project
set( HEADERS
    src/core/ProjectDefines.h
    src/core/CommonFunctions.h
    src/core/Component.h
    src/core/Logger.h
    src/core/Callbacks.h
    src/core/Timer.h
    src/core/EventLocker.h
    src/core/Tunnel.h
    src/testcases/TestCase.h
    src/testcases/SystemInfo.h
    src/testcases/ListComponents.h
    src/testcases/PortInfo.h
    src/testcases/NoBuffers.h
    src/testcases/ImageDecode.h
    src/testcases/BuffersNoIL.h
    src/testcases/VideoDecodeRender.h
    src/components/DecoderH264.h
    src/components/VideoRenderer.h
)

# List of all source files that belong to this project
set( SOURCES
    main.cpp
    src/core/CommonFunctions.cpp
    src/core/Component.cpp
    src/core/Logger.cpp
    src/core/Timer.cpp
    src/core/EventLocker.cpp
    src/core/Tunnel.cpp
    src/testcases/TestCase.cpp
    src/testcases/SystemInfo.cpp
    src/testcases/ListComponents.cpp
    src/testcases/PortInfo.cpp
    src/testcases/NoBuffers.cpp
    src/testcases/ImageDecode.cpp
    src/testcases/BuffersNoIL.cpp
    src/testcases/VideoDecodeRender.cpp
    src/components/DecoderH264.cpp
    src/components/VideoRenderer.cpp
)

# Create executable with all necessary source and header files
add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)

# pthreads things
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Include directories
INCLUDE_DIRECTORIES(
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/include
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/include/interface/vcos/pthreads
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/include/interface/vmcs_host/linux
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/src/hello_pi/libs/ilclient/
)

# Link all needed libraries
TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libbcm_host.so
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libopenmaxil.so
  Threads::Threads
)
