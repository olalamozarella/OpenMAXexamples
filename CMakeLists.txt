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

# Set library name
set( LIB_NAME OMXHelperLib )

# header files for library
set( LIB_HEADERS
    src/core/ProjectDefines.h
    src/core/CommonFunctions.h
    src/core/Component.h
    src/core/Logger.h
    src/core/Callbacks.h
    src/core/Timer.h
    src/core/EventLocker.h
    src/core/Tunnel.h
    src/core/ThreadWorker.h
    src/threadworkers/FileReader.h
    src/threadworkers/FileWriter.h
    src/components/DecoderH264.h
    src/components/VideoRenderer.h
    src/components/DecoderMPEG2.h
    src/components/DecoderJPEG.h
    src/components/EncoderJPEG.h
    src/components/EncoderH264.h
)

# source files for library
set( LIB_SOURCES
    src/core/CommonFunctions.cpp
    src/core/Component.cpp
    src/core/Logger.cpp
    src/core/Timer.cpp
    src/core/EventLocker.cpp
    src/core/Tunnel.cpp
    src/core/ThreadWorker.cpp
    src/threadworkers/FileReader.cpp
    src/threadworkers/FileWriter.cpp
    src/components/DecoderH264.cpp
    src/components/VideoRenderer.cpp
    src/components/DecoderMPEG2.cpp
    src/components/DecoderJPEG.cpp
    src/components/EncoderJPEG.cpp
    src/components/EncoderH264.cpp
)

# List of all header files that belong to this project
set( BINARY_HEADERS
    src/testcases/TestCase.h
    src/testcases/SystemInfo.h
    src/testcases/ListComponents.h
    src/testcases/PortInfo.h
    src/testcases/NoBuffers.h
    src/testcases/BuffersNoIL.h
    src/testcases/VideoDecodeRender.h
    src/testcases/MPEG2Stuff.h
    src/testcases/ImageDecodeRender.h
    src/testcases/EncodeImage.h
    src/testcases/EncodeVideo.h
    src/testcases/DecodeVideo.h
)

# List of all source files that belong to this project
set( BINARY_SOURCES
    main.cpp
    src/testcases/TestCase.cpp
    src/testcases/SystemInfo.cpp
    src/testcases/ListComponents.cpp
    src/testcases/PortInfo.cpp
    src/testcases/NoBuffers.cpp
    src/testcases/BuffersNoIL.cpp
    src/testcases/VideoDecodeRender.cpp
    src/testcases/MPEG2Stuff.cpp
    src/testcases/ImageDecodeRender.cpp
    src/testcases/EncodeImage.cpp
    src/testcases/EncodeVideo.cpp
    src/testcases/DecodeVideo.cpp
)

# Create library with core&components
#add_library(
#    ${LIB_NAME}
#    SHARED
#    ${LIB_HEADERS}
#    ${LIB_SOURCES}
#)

# Create executable with all necessary source and header files
add_executable(${PROJECT_NAME}
    ${LIB_HEADERS}
    ${LIB_SOURCES}
    ${BINARY_HEADERS}
    ${BINARY_SOURCES}
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
#  ${LIB_NAME}
)

link_directories( ${CMAKE_CURRENT_BINARY_DIR} )
