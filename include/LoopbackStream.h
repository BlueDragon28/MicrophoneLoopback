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