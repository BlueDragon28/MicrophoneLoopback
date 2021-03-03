#include "LoopbackStream.h"
#include <iostream>
#include <cstring>
#include <thread>

LoopbackStream::LoopbackStream() :
    m_channelsCount(1),
    m_sampleRate(48000),
    m_sizePerSample(2),
    m_streamFramePerBuffer(800),

    m_isStreamReady(false),
    m_inputStream(nullptr),
    m_outputStream(nullptr),
    m_isPlayingContinue(false),

    m_inputBuffer1(nullptr),
    m_inputBuffer2(nullptr),
    m_inputBufferSize(m_streamFramePerBuffer * m_sizePerSample * m_channelsCount),
    m_inputIsBuffer1(true),
    m_ouputIsBuffer1(false)
{}

LoopbackStream::~LoopbackStream()
{
    deinit();
}

void LoopbackStream::deinit()
{
    stop();

    // Deinitialization of the input and output stream.
    if (m_inputStream)
    {
        Pa_CloseStream(m_inputStream);
        m_inputStream = nullptr;
    }
    if (m_outputStream)
    {
        Pa_CloseStream(m_outputStream);
        m_outputStream = nullptr;
    }

    // Destroy the buffer
    if (m_inputBuffer1)
    {
        delete[] m_inputBuffer1;
        m_inputBuffer1 = nullptr;
    }
    if (m_inputBuffer2)
    {
        delete[] m_inputBuffer2;
        m_inputBuffer2 = nullptr;
    }
    
    m_isStreamReady = false;
    m_isPlayingContinue = false;
}

bool LoopbackStream::init()
{
    // First, dinitialization of any existing stream.
    deinit();

    int err = paNoError;

    // Creating the input stream with the default input device.
    err = Pa_OpenDefaultStream(
        &m_inputStream,
        m_channelsCount,
        0,
        paInt16,
        static_cast<double>(m_sampleRate),
        m_streamFramePerBuffer,
        LoopbackStream::staticInputCallback,
        static_cast<void*>(this));

    if (err != paNoError)
    {
        m_isStreamReady = false;
        m_isPlayingContinue = false;
        m_strError = "Failed to create the input stream.";
        return false;
    }

    // Creating the output stream with the default output device.
    err = Pa_OpenDefaultStream(
        &m_outputStream,
        0,
        m_channelsCount,
        paInt16,
        static_cast<double>(m_sampleRate),
        m_streamFramePerBuffer,
        LoopbackStream::staticOutputCallback,
        static_cast<void*>(this));

    if (err != paNoError)
    {
        m_isStreamReady = false;
        m_isPlayingContinue = false;
        m_strError = "Failed to create the output stream.";
        return false;
    }

    // Creating the input and output stream
    m_inputBuffer1 = new char[m_inputBufferSize];
    memset(m_inputBuffer1, 0, m_inputBufferSize);
    m_inputBuffer2 = new char[m_inputBufferSize];
    memset(m_inputBuffer2, 0, m_inputBufferSize);

    // Everything is fine.
    m_isStreamReady = true;
    return true;
}

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
    return lStream->inputCallback(inputBuffer);
}

int LoopbackStream::staticOutputCallback(
    const void *inputBuffer,
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData
)
{
    // redirectint this function to the member function of LoopbackStream.
    LoopbackStream* lStream = static_cast<LoopbackStream*>(userData);
    return lStream->outputCallback(outputBuffer);
}

int LoopbackStream::inputCallback(const void* inputBuffer)
{
    // Copy the buffer from the microphone to the member variable m_inputBuffer.
    if (m_inputIsBuffer1)
    {
        m_inputBuffer1Locker.lock();
        memcpy(m_inputBuffer1, inputBuffer, m_inputBufferSize);
        m_inputIsBuffer1 = false;
        m_inputBuffer1Locker.unlock();
    }
    else
    {
        m_inputBuffer2Locker.lock();
        memcpy(m_inputBuffer2, inputBuffer, m_inputBufferSize);
        m_inputIsBuffer1 = true;
        m_inputBuffer2Locker.unlock();
    }
    return paContinue;
}

int LoopbackStream::outputCallback(void* outputBuffer)
{
    // Copy the member buffer m_inputBuffer to the buffer use by the speakers.
    if (m_ouputIsBuffer1)
    {
        m_inputBuffer1Locker.lock();
        memcpy(outputBuffer, m_inputBuffer1, m_inputBufferSize);
        m_ouputIsBuffer1 = false;
        m_inputBuffer1Locker.unlock();
    }
    else
    {
        m_inputBuffer2Locker.lock();
        memcpy(outputBuffer, m_inputBuffer2, m_inputBufferSize);
        m_ouputIsBuffer1 = true;
        m_inputBuffer2Locker.unlock();
    }
    return paContinue;
}

bool LoopbackStream::play()
{
    // Playing the input and output stream.
    if (m_isStreamReady)
    {
        int err = Pa_StartStream(m_inputStream);
        if (err != paNoError)
        {
            m_strError = "Failed to start the input stream.";
            m_isPlayingContinue = false;
            return false;
        }
        err = Pa_StartStream(m_outputStream);
        if (err != paNoError)
        {
            m_strError = "Failed to start the output stream.";
            m_isPlayingContinue = true;
            return false;
        }

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
    if (m_inputStream)
        Pa_StopStream(m_inputStream);
    if (m_outputStream)
        Pa_StopStream(m_outputStream);
}

bool LoopbackStream::isStreamReady() const
{
    return m_isStreamReady;
}

bool LoopbackStream::isPlayingContinue() const
{
    return m_isPlayingContinue;
}