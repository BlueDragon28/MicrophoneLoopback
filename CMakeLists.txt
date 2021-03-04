cmake_minimum_required(VERSION 3.16.0)
project(MicrophoneLoopback LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/)

if (WIN32)
    #set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/cmake;${CMAKE_INSTALL_PREFIX}")
    set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/cmake;${CMAKE_PREFIX_PATH}")
    include_directories("${CMAKE_SOURCE_DIR}/dependencies/portaudio/include")
elseif(LINUX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PORTAUDIO REQUIRED portaudiocpp)
endif()

add_executable(MicrophoneLoopback
    "src/main.cpp"
    "include/LoopbackStream.h"
    "include/StreamApplication.h"
    "src/LoopbackStream.cpp"
    "src/StreamApplication.cpp")
if(WIN32)
    if (CMAKE_CL_64)
        target_link_libraries(MicrophoneLoopback "${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/portaudio_static_x64.lib")
    else()
        target_link_libraries(MicrophoneLoopback "${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/portaudio_static.lib")
    endif()
elseif(LINUX)
    target_link_libraries(MicrophoneLoopback ${PORTAUDIO_LIBRARIES})
endif()
set_target_properties(MicrophoneLoopback PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
