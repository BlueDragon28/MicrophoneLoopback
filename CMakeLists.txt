cmake_minimum_required(VERSION 3.16.0)
cmake_policy(SET CMP0091 NEW)
project(MicrophoneLoopback LANGUAGES CXX)

# Set C++11 has default
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/)

if (WIN32)
    include_directories("${CMAKE_SOURCE_DIR}/dependencies/portaudio/include")
elseif(UNIX AND NOT APPLE)
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
    set_property(TARGET MicrophoneLoopback PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(UNIX AND NOT APPLE)
    if (${PORTAUDIO_STATIC_BUILD})
        target_link_libraries(MicrophoneLoopback 
            ${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/libportaudio.a 
            -ljack 
            -lasound 
            -lm 
            -lpthread)
        message("-- Compiling MicorphoneLoopback statically with user portaudio.")
    else()
    endif()
endif()
set_target_properties(MicrophoneLoopback PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
