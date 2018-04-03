### Cmake file for simple RPi projects ###

#Reload cmake toolchain file with "Run CMake", parameters:
#cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=/opt/rpi-cross-compile-essentials/pi.cmake ./

#Add every class to HEADERS and SOURCES, otherwise it wont be displayed in QTCreator project.
#Add external libraries with TARGET_LINK_LIBRARIES()

cmake_minimum_required(VERSION 3.1)
project(cmake-test-qtcreator)
set(CMAKE_INSTALL_PREFIX ".")
set(CMAKE_INSTALL_RPATH "$ORIGIN/")

# Useful CMake options for Qt projects
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Compile flags
set(CMAKE_BUILD_TYPE Debug)
set(COMPILE_DEFINITIONS -Werror)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRASPBERRYPI -DOMX_SKIP64BIT")

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
    src/core/TunnelEmulator.h
    src/threadworkers/FileReader.h
    src/threadworkers/FileWriter.h
    src/components/DecoderH264.h
    src/components/VideoRenderer.h
    src/components/DecoderMPEG2.h
    src/components/DecoderJPEG.h
    src/components/EncoderJPEG.h
    src/components/EncoderH264.h
    src/components/NullSink.h
    src/components/Camera.h
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
    src/components/NullSink.cpp
    src/components/Camera.cpp
)

# Create library with core&components
set(LIBRARY_NAME OmxHelperLib)
add_library(${LIBRARY_NAME} SHARED ${LIB_HEADERS} ${LIB_SOURCES})
install(TARGETS ${LIBRARY_NAME} DESTINATION bin)
install(FILES ${LIB_HEADERS} DESTINATION include)

# List of all header files that belong to this project
#set( BINARY_HEADERS
#    src/testcases/VideoDecodeRender.h
#    src/testcases/ImageDecodeRender.h
#    src/testcases/MPEG2Stuff.h
#    src/testcases/DecodeVideoNullSink.h
#    src/testcases/EncodeVideoNullSink.h
#)

# List of all source files that belong to this project
#set( BINARY_SOURCES
#    main.cpp
#    src/testcases/VideoDecodeRender.cpp
#    src/testcases/ImageDecodeRender.cpp
#    src/testcases/MPEG2Stuff.cpp
#    src/testcases/DecodeVideoNullSink.cpp
#    src/testcases/EncodeVideoNullSink.cpp
#)

# Create executables for individual testcases
add_executable(CameraRender src/testcases/CameraRender.h src/testcases/CameraRender.cpp)
install(TARGETS CameraRender DESTINATION bin)
target_link_libraries( CameraRender ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libbcm_host.so
    ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libopenmaxil.so Threads::Threads ${LIBRARY_NAME})

add_executable(CameraEncode src/testcases/CameraEncode.h src/testcases/CameraEncode.cpp)
install(TARGETS CameraEncode DESTINATION bin)
target_link_libraries( CameraEncode ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libbcm_host.so
    ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libopenmaxil.so Threads::Threads ${LIBRARY_NAME})

add_executable(EncodeImage src/testcases/EncodeImage.h src/testcases/EncodeImage.cpp)
install(TARGETS EncodeImage DESTINATION bin)
target_link_libraries( EncodeImage ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libbcm_host.so
    ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libopenmaxil.so Threads::Threads ${LIBRARY_NAME})

add_executable(DecodeImage src/testcases/DecodeImage.h src/testcases/DecodeImage.cpp)
install(TARGETS DecodeImage DESTINATION bin)
target_link_libraries( DecodeImage ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libbcm_host.so
    ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libopenmaxil.so Threads::Threads ${LIBRARY_NAME})

add_executable(EncodeVideo src/testcases/EncodeVideo.h src/testcases/EncodeVideo.cpp)
install(TARGETS EncodeVideo DESTINATION bin)
target_link_libraries( EncodeVideo ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libbcm_host.so
    ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libopenmaxil.so Threads::Threads ${LIBRARY_NAME})

add_executable(DecodeVideo src/testcases/DecodeVideo.h src/testcases/DecodeVideo.cpp)
install(TARGETS DecodeVideo DESTINATION bin)
target_link_libraries( DecodeVideo ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libbcm_host.so
    ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libopenmaxil.so Threads::Threads ${LIBRARY_NAME})

# pthreads things
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Include directories
include_directories(
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/include
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/include/interface/vcos/pthreads
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/include/interface/vmcs_host/linux
)

link_directories( ${CMAKE_CURRENT_BINARY_DIR} )
