#include "LoopbackStream.h"
#include <cstring>

LoopbackStream::LoopbackStream() :
    m_channelsCount(1),
    m_sampleRate(48000),
    m_sizePerSample(2),
    m_streamFramePerBuffer(256),
#ifdef WIN32
    m_stream(nullptr),
#endif
    m_isStreamReady(false),
    m_isPlayingContinue(false),
    m_inputBufferSize(m_streamFramePerBuffer * m_sizePerSample * m_channelsCount)
{}

LoopbackStream::~LoopbackStream()
{
    deinit();
}

void LoopbackStream::deinit()
{
    stop();

    // Deinitialization of the stream.
#ifdef WIN32
    if (m_stream)
    {
        Pa_CloseStream(m_stream);
        m_stream = nullptr;
    }
#endif
    
    m_isStreamReady = false;
    m_isPlayingContinue = false;
}

bool LoopbackStream::init()
{
    // First, dinitialization of any existing stream.
    deinit();

#ifdef WIN32
    int err = paNoError;

    // Creating the input stream with the default input device.
    PaStreamParameters inputStreamParams = {};
    inputStreamParams.device = Pa_GetDefaultInputDevice();
    inputStreamParams.channelCount = m_channelsCount;
    inputStreamParams.sampleFormat = paInt16;
    inputStreamParams.suggestedLatency = 0.2;
    inputStreamParams.hostApiSpecificStreamInfo = nullptr;

    PaStreamParameters outputStreamParams = {};
    outputStreamParams.device = Pa_GetDefaultOutputDevice();
    outputStreamParams.channelCount = m_channelsCount;
    outputStreamParams.sampleFormat = paInt16;
    outputStreamParams.suggestedLatency = 0.2;
    outputStreamParams.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(
        &m_stream,
        &inputStreamParams,
        &outputStreamParams,
        m_sampleRate,
        m_streamFramePerBuffer,
        paClipOff,
        LoopbackStream::staticInputCallback,
        static_cast<void*>(this));

    if (err != paNoError)
    {
        m_isStreamReady = false;
        m_isPlayingContinue = false;
        m_strError = "Failed to create the input stream.";
        return false;
    }
#endif

    // Everything is fine.
    m_isStreamReady = true;
    return true;
}

#ifdef WIN32
int LoopbackStream::staticInputCallback(
    const void *inputBuffer,
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
    // redirectint this function to the member function of LoopbackStream.
    LoopbackStream* lStream = static_cast<LoopbackStream*>(userData);
    return lStream->inputCallback(inputBuffer, outputBuffer);
}

int LoopbackStream::inputCallback(const void* inputBuffer, void* outputBuffer)
{
    memcpy(outputBuffer, inputBuffer, m_inputBufferSize);
    return paContinue;
}
#endif

bool LoopbackStream::play()
{
    // Playing the stream
    if (m_isStreamReady)
    {
#ifdef WIN32
        int err = Pa_StartStream(m_stream);
        if (err != paNoError)
        {
            m_strError = "Failed to start the input stream.";
            m_isPlayingContinue = false;
            return false;
        }
#endif

        m_isPlayingContinue = true;
        return true;
    }
    else
    {
        m_strError = "The stream is not ready.";
        return false;
    }
}

void LoopbackStream::stop()
{
    // Stopping the stream.
#ifdef WIN32
    if (m_stream)
        Pa_StopStream(m_stream);
#endif
}

bool LoopbackStream::isStreamReady() const
{
    return m_isStreamReady;
}

bool LoopbackStream::isPlayingContinue() const
{
    return m_isPlayingContinue;
}