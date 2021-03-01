cmake_minimum_required(VERSION 3.16.0)
project(MicrophoneLoopback LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(PORTAUDIO REQUIRED portaudiocpp)

add_executable(MicrophoneLoopback
    "src/main.cpp")
target_link_libraries(MicrophoneLoopback ${PORTAUDIO_LIBRARIES})
set_target_properties(MicrophoneLoopback PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
