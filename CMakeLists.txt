# MIT Licence

# MicrophoneLoopback

# Copyright © 2022 Erwan Saclier de la Bâtie (BlueDragon28)

# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
# to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
# and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.16.0)
cmake_policy(SET CMP0091 NEW)
project(MicrophoneLoopback LANGUAGES CXX)

# Set C++11 has default
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/)

if (WIN32)
    # Check if portaudio library is located in dependencies folder
    if (NOT EXISTS "${CMAKE_SOURCE_DIR}/dependencies/portaudio/include/portaudio.h")
        message (FATAL_ERROR "Failed to find PortAudio include file.")
    endif ()

    if (EXISTS "${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/portaudio_static_x64.lib")
        message (STATUS "Found portaudio_static_x64.lib")
        set(PORTAUDIO_LIB "${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/portaudio_static_x64.lib")
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/portaudio_static.lib")
        message (STATUS "Found portaudio_static.lib")
        set(PORTAUDIO_LIB "${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/portaudio_static.lib")
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/portaudio_x64.lib")
        message (STATUS "Found portaudio_x64.lib")
        set(PORTAUDIO_LIB "${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/portaudio_x64.lib")
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/portaudio.lib")
        message (STATUS "Found portaudio.lib")
        set(PORTAUDIO_LIB "${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/portaudio.lib")
    else()
        message(FATAL_ERROR "Cannot find PortAudio!")
    endif ()
    include_directories("${CMAKE_SOURCE_DIR}/dependencies/portaudio/include")

    # Check if cxxopts library is located in dependencies folder.
    if (NOT EXISTS "${CMAKE_SOURCE_DIR}/dependencies/cxxopts/include/cxxopts.hpp")
        message(FATAL_ERROR "Failed to find cxxopts include file.")
    endif()
    include_directories("${CMAKE_SOURCE_DIR}/dependencies/cxxopts/include")

    if ((NOT EXISTS "${CMAKE_SOURCE_DIR}/dependencies/ini_parser/include/ini_parser.h") OR
        (NOT EXISTS "${CMAKE_SOURCE_DIR}/dependencies/ini_parser/src/ini_parser.cpp"))
        message(FATAL_ERROR "Failed to find the ini_parser library files")
    endif()
    include_directories("${CMAKE_SOURCE_DIR}/dependencies/ini_parser/include/")
elseif(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PULSE REQUIRED libpulse-simple)
    pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
    pkg_check_modules(INI_PARSER REQUIRED ini_parser_static)
    include_directories(${INI_PARSER_INCLUDE_DIRS})
    link_directories("${INI_PARSER_LIBRARY_DIRS}")
endif()

if (WIN32)
    add_executable(MicrophoneLoopback
        "src/main.cpp"
        "include/LoopbackStream.h"
        "include/StreamApplication.h"
        "include/CMDParser.h"
        "src/LoopbackStream.cpp"
        "src/StreamApplication.cpp"
        "src/CMDParser.cpp"
        "include/Common.h"
        "src/Common.cpp"
        "${CMAKE_SOURCE_DIR}/dependencies/ini_parser/src/ini_parser.cpp")
else()
add_executable(MicrophoneLoopback
        "src/main.cpp"
        "include/LoopbackStream.h"
        "include/StreamApplication.h"
        "include/CMDParser.h"
        "src/LoopbackStream.cpp"
        "src/StreamApplication.cpp"
        "src/CMDParser.cpp"
        "include/Common.h"
        "src/Common.cpp")
endif()
if(WIN32)
    target_link_libraries(MicrophoneLoopback "${PORTAUDIO_LIB}")
    set_property(TARGET MicrophoneLoopback PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(UNIX AND NOT APPLE)
    if (${PORTAUDIO_STATIC_BUILD})
        target_link_libraries(MicrophoneLoopback 
            ${CMAKE_SOURCE_DIR}/dependencies/portaudio/lib/libportaudio.a
            ${PULSE_LIBRARIES}
            ${INI_PARSER_LIBRARIES}
            -ljack 
            -lasound 
            -lm 
            -lpthread)
        message("-- Compiling MicorphoneLoopback statically with user portaudio.")
    else()
        target_link_libraries(MicrophoneLoopback ${PULSE_LIBRARIES} ${PORTAUDIO_LIBRARIES} ${INI_PARSER_LIBRARIES} -lpthread)
    endif()
endif()
set_target_properties(MicrophoneLoopback PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
