#ifndef LOOPBACKSTREAM_MLB_H
#define LOOPBACKSTREAM_MLB_H

#include <portaudio.h>
#include <string>
#include <mutex>

class LoopbackStream
{
    // Disabling copy constructor
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
    // Static callbacks used has interface to C callbacks
    static int staticInputCallback(
        const void *inputBuffer,
        void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData
    );
    static int staticOutputCallback(
        const void *inputBuffer,
        void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData
    );

    // Callbacks
    int inputCallback(
        const void *inputBuffer
    );
    int outputCallback(
        void* outputBuffer
    );

private:
    std::string m_strError;

    // Stream information
    int m_channelsCount, m_sampleRate, m_sizePerSample;
    unsigned long m_streamFramePerBuffer;

    // Initialisation variables.
    bool m_isStreamReady;
    PaStream *m_inputStream,
             *m_outputStream;

    // Playing variables.
    bool m_isPlayingContinue;

    // Buffer data
    bool m_inputIsBuffer1, m_ouputIsBuffer1;
    char *m_inputBuffer1, *m_inputBuffer2;
    std::mutex m_inputBuffer1Locker, m_inputBuffer2Locker;
    size_t m_inputBufferSize;
};

#endif // LOOPBACKSTREAM_MLB_H