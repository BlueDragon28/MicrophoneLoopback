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

#ifndef LOOPBACKSTREAM_MLB_H
#define LOOPBACKSTREAM_MLB_H

#ifdef WIN32
#include <portaudio.h>
#elif __linux__
#include <pulse/simple.h>
#include <thread>
#endif
#include <string>

class LoopbackStream
{
    // Disabling the copy constructor
    LoopbackStream(const LoopbackStream&) = delete;
public:
    LoopbackStream();
    ~LoopbackStream();

    // Initialize the stream.
    bool init();
    // Deinit streams.
    void deinit();

    // Start playing
    bool play();
    void stop();

    bool isStreamReady() const; // Is the stream is ready to play.
    bool isPlayingContinue() const; // Is the stream is playing.

private:
#ifdef WIN32
    // Static callbacks used has interface to C callbacks
    static int staticInputCallback(
        const void *inputBuffer,
        void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData
    );

    // Callbacks
    int inputCallback(
        const void *inputBuffer,
        void* outputBuffer
    );

#elif __linux__
    void streamLoop();
    void readingStream(int* index);
#endif

private:
    std::string m_strError;

    // Stream information
    int m_channelsCount, m_sampleRate, m_sizePerSample;
    unsigned long m_streamFramePerBuffer;

    // Stream.
    bool m_isStreamReady;
#ifdef WIN32
    PaStream *m_stream;
#elif __linux__
    pa_simple* m_inputStream;
    pa_simple* m_outputStream;
    std::thread m_tStream;
#endif

    // Playing variables.
    bool m_isPlayingContinue;

    // Buffer size
    size_t m_inputBufferSize;

    // Buffer data
#ifdef __linux__
    char* m_data;
    char* m_data2;
#endif
};

#endif // LOOPBACKSTREAM_MLB_H