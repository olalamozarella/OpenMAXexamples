### Cmake file for simple RPi projects ###

#Reload cmake toolchain file with "Run CMake", parameters:
#cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=/opt/rpi-cross-compile-essentials/pi.cmake ./

#Add every class to HEADERS and SOURCES, otherwise it wont be displayed in QTCreator project.
#Add external libraries with TARGET_LINK_LIBRARIES()

cmake_minimum_required(VERSION 2.8)

project(cmake-test-qtcreator)

# Useful CMake options for Qt projects
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Compile flags
set(CMAKE_BUILD_TYPE Debug)
set(COMPILE_DEFINITIONS -Werror)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRASPBERRYPI -DOMX_SKIP64BIT")

# List of all header files that belong to this project
set( HEADERS
    src/ProjectDefines.h
    src/CommonFunctions.h
    src/testcases/TestCase.h
    src/testcases/SystemInfo.h
    src/testcases/ListComponents.h
    src/testcases/PortInfo.h
    src/testcases/NoBuffers.h
    src/testcases/ImageDecode.h
)

# List of all source files that belong to this project
set( SOURCES
    main.cpp
    src/CommonFunctions.cpp
    src/testcases/TestCase.cpp
    src/testcases/SystemInfo.cpp
    src/testcases/ListComponents.cpp
    src/testcases/PortInfo.cpp
    src/testcases/NoBuffers.cpp
    src/testcases/ImageDecode.cpp
)

# Create executable with all necessary source and header files
add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)

# Include directories
INCLUDE_DIRECTORIES(
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/include
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/include/interface/vcos/pthreads
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/include/interface/vmcs_host/linux
)

# Link all needed libraries
TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libbcm_host.so
  ${CMAKE_FIND_ROOT_PATH}/opt/vc/lib/libopenmaxil.so
)
