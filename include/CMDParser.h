/*
 * MIT Licence
 * 
 * MicrophoneLoopback
 * 
 * Copyright © 2022 Erwan Saclier de la Bâtie (BlueDragon28)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef STREAMAPPLICATION_CMDParser
#define STREAMAPPLICATION_CMDParser

#include "Common.h"
#include <string>
#include <cxxopts.hpp>

class CMDParser
{
public: 
    CMDParser(int& argc, char**& argv);
    ~CMDParser();

    bool isSampleRateSet() const;
    int sampleRate() const;
    bool isFramesPerBufferSet() const;
    int framesPerBuffer() const;

#ifdef WIN32
    bool isInputLatencySet() const;
    double inputLatency() const;
    bool isOutputLatencySet() const;
    double outputLatency() const;
#elif __linux__
    bool usePortAudio() const;
#endif

    bool isBackendSet() const;
    BackendAudio backend() const;

private:
    bool m_isSampleRateSet;
    int m_sampleRate;
    bool m_isframesPerBufferSet;
    int m_framesPerBuffer;

#ifdef WIN32
    bool m_isInputLatencySet;
    double m_inputLatency;
    bool m_isOutputLatencySet;
    double m_outputLatency;
#elif __linux__
    bool m_usePortAudio;
#endif

    bool m_isBackendSet;
    BackendAudio m_backend;
};

#endif // STREAMAPPLICATION_CMDParser
