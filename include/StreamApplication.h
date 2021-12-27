/*
 * MIT Licence
 * 
 * MicrophoneLoopback
 * 
 * Copyright © 2021 Erwan Saclier de la Bâtie (Erwan28250)
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

#ifndef STREAMAPPLICATION_MLB_H
#define STREAMAPPLICATION_MLB_H

#include "LoopbackStream.h"

#ifdef WIN32
#include "windows.h"
#endif

class StreamApplication
{
    StreamApplication(const StreamApplication& other) = delete;
public:
    StreamApplication(int& argc, char**& argv);
    ~StreamApplication();

    // Setting the LoopbackStream pointer.
    // The application will use the stream to known if it
    // need to close.
    void setStream(LoopbackStream* stream);

    bool isAppContinue() const;
    bool isAppReady() const;

    int run();

    void stopApplication();

private:
    void deinit();

#ifdef WIN32
    void createWindowsSignalsCatch();
    static BOOL WINAPI windowsSignalsHandler(DWORD signal);
#elif __linux__
    // Linux handler for catch ctrl-c and term signal.
    void createSigAction();
    static void sigActionHandler(int signal);
#endif

    LoopbackStream* m_stream;
    bool m_isAppContinue;
    bool m_isAppReady;
    int m_sampleRate;
    int m_framesPerBuffer;
#ifdef WIN32
    double m_inputLatency;
    double m_outputLatency;
#endif
};

#endif // STREAMAPPLICATION_MLB_H