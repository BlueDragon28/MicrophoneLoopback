cmake_minimum_required(VERSION 3.16.0)
cmake_policy(SET CMP0091 NEW)
project(MicrophoneLoopback LANGUAGES CXX)

# Set C++11 has default
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/)

# This option is used to compile statically portaudio with a custom build portaudio set by the users.
# The folder with the compiled portaudio must be place in dependencies folder and renamed to portaudio.
if (UNIX AND NOT APPLE)
    option(PORTAUDIO_STATIC_BUILD "Build with user portaudio package set in dependencies/portaudio folders. Otherwise, it's use package distribution." ON)
endif()

if (WIN32)
    include_directories("${CMAKE_SOURCE_DIR}/dependencies/portaudio/include")
elseif(UNIX AND NOT APPLE)
    if (NOT ${PORTAUDIO_STATIC_BUILD})
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(PORTAUDIO REQUIRED portaudiocpp)
    endif()
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
        target_link_libraries(MicrophoneLoopback ${PORTAUDIO_LIBRARIES})
        message("-- Compiling MicrophoneLoopback with the distribution portaudio package.")
    endif()
endif()
set_target_properties(MicrophoneLoopback PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
