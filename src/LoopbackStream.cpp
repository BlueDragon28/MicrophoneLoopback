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

#include "LoopbackStream.h"
#include "Common.h"
#include <cstring>
#include <iostream>
#include <portaudio.h>

LoopbackStream::LoopbackStream() :
    m_channelsCount(1),
    m_sampleRate(48000),
    m_sizePerSample(2),
    m_streamFramePerBuffer(256),
#ifdef WIN32
    m_inputLatency(0.02),
    m_outputLatency(0.02),
#endif
    m_stream(nullptr),
#ifdef __linux__
    m_usePortAudio(false),
    m_inputStream(nullptr),
    m_outputStream(nullptr),
#endif
    m_isStreamReady(false),
    m_isPlayingContinue(false),
    m_inputBufferSize(m_streamFramePerBuffer * m_sizePerSample * m_channelsCount),
#ifdef __linux__
    m_data(nullptr),
#endif
    m_availableBackend(getAvailablesBackend())
{}

LoopbackStream::~LoopbackStream()
{
    deinit();
}

void LoopbackStream::deinit()
{
    stop();

    // Deinitialization of the streams and datas.
    if (m_stream)
    {
        Pa_CloseStream(m_stream);
        m_stream = nullptr;
    }
#ifdef __linux__
    if (m_tStream.joinable())
        m_tStream.join();

    if (m_inputStream)
    {
        pa_simple_free(m_inputStream);
        m_inputStream = nullptr;
    }
    if (m_outputStream)
    {
        pa_simple_free(m_outputStream);
        m_outputStream = nullptr;
    }
    if (m_data)
    {
        delete[] m_data;
        m_data = nullptr;
    }
#endif
    
    m_isStreamReady = false;
    m_isPlayingContinue = false;
}

bool LoopbackStream::init()
{
    // First, dinitialization of any existing stream.
    deinit();

#ifdef __linux__
    if (m_usePortAudio)
    {
#endif

    int err = paNoError;

    const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(
            Pa_HostApiTypeIdToHostApiIndex((PaHostApiTypeId)fromBackendToHostApiID(m_backend)));

    if (!hostApiInfo)
    {
        std::cerr << "Cannot get host api info!" << std::endl;
        std::exit(-1);
    }

    if (hostApiInfo->defaultInputDevice == paNoDevice)
    {
        std::cerr << "No default input device" << std::endl;
        std::exit(-1);
    }

    if (hostApiInfo->defaultOutputDevice == paNoDevice)
    {
        std::cerr << "No default output device" << std::endl;
        std::exit(-1);
    }

    // Creating the input stream with the default input device.
    PaStreamParameters inputStreamParams = {};
    inputStreamParams.device = hostApiInfo->defaultInputDevice;
    inputStreamParams.channelCount = m_channelsCount;
    inputStreamParams.sampleFormat = paInt16;
#ifdef WIN32
    inputStreamParams.suggestedLatency = m_inputLatency;
#elif __linux__
    inputStreamParams.suggestedLatency = 0.;
#endif
    inputStreamParams.hostApiSpecificStreamInfo = nullptr;

    PaStreamParameters outputStreamParams = {};
    outputStreamParams.device = hostApiInfo->defaultOutputDevice;
    outputStreamParams.channelCount = m_channelsCount;
    outputStreamParams.sampleFormat = paInt16;
#ifdef WIN32
    outputStreamParams.suggestedLatency = m_outputLatency;
#elif __linux__
    outputStreamParams.suggestedLatency = 0.;
#endif
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

#ifdef __linux__
    }
    else
    {
        // Stream specification.
        pa_sample_spec sampleSpec;
        sampleSpec.channels = m_channelsCount;
        sampleSpec.format = PA_SAMPLE_S16LE;
        sampleSpec.rate = m_sampleRate;

        // Pulseaudio buffer length.
        pa_buffer_attr bufferAtribute;
        bufferAtribute.maxlength = m_inputBufferSize;
        bufferAtribute.tlength = -1;
        bufferAtribute.prebuf = -1;
        bufferAtribute.minreq = -1;
        bufferAtribute.fragsize = -1;

        // Opening the input stream. (from the microphone.)
        m_inputStream = pa_simple_new(
            nullptr,
            "MicrophoneLoopback",
            PA_STREAM_RECORD,
            nullptr,
            "Microphone record",
            &sampleSpec,
            nullptr,
            &bufferAtribute,
            nullptr
        );

        if (!m_inputStream)
        {
            m_strError = "Failed to start the input stream.";
            m_isStreamReady = false;
            m_isPlayingContinue = false;
            return false;
        }

        // Opening the output stream. (to the speakers.)
        m_outputStream = pa_simple_new(
            nullptr,
            "MicrophoneLoopback",
            PA_STREAM_PLAYBACK,
            nullptr,
            "Microphone playback",
            &sampleSpec,
            nullptr,
            &bufferAtribute,
            nullptr
        );

        if (!m_outputStream)
        {
            m_strError = "Failed to start the output stream.";
            m_isStreamReady = false;
            m_isPlayingContinue = false;
            return false;
        }

        // Creating the two temporaring buffers.
        m_data = new char[m_inputBufferSize];
        memset(m_data, 0, m_inputBufferSize);
    }
#endif

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
    return lStream->inputCallback(inputBuffer, outputBuffer);
}

int LoopbackStream::inputCallback(const void* inputBuffer, void* outputBuffer)
{
    memcpy(outputBuffer, inputBuffer, m_inputBufferSize);
    return paContinue;
}

#ifdef __linux__
void LoopbackStream::streamLoop()
{
    // Starting to play
    int err = PA_OK;
    while (m_isPlayingContinue)
    {
        // Write the data to the playback buffer.
        err = pa_simple_write(m_outputStream, m_data, m_inputBufferSize, nullptr);
        if (err != 0)
        {
            m_strError = "Failed to play data.";
            m_isPlayingContinue = false;
            break;
        }
        err = pa_simple_read(m_inputStream, m_data, m_inputBufferSize, nullptr);
        if (err != 0)
        {
            m_strError = "Failed to read data from the microphone.";
            m_isPlayingContinue = false;
            break;
        }
    }
}
#endif

bool LoopbackStream::play()
{
    // Playing the stream
    if (m_isStreamReady)
    {
#ifdef __linux__
        if (m_usePortAudio)
        {
#endif

        int err = Pa_StartStream(m_stream);
        if (err != paNoError)
        {
            m_strError = "Failed to start the input stream.";
            m_isPlayingContinue = false;
            return false;
        }

        m_isPlayingContinue = true;

#ifdef __linux__
        } 
        else 
        {
            // Launch the loop of the stream into another thread.
            m_tStream = std::thread(&LoopbackStream::streamLoop, this);
            m_isPlayingContinue = true;
        }
#endif

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
    if (m_stream)
        Pa_StopStream(m_stream);
    m_isPlayingContinue = false;
}

bool LoopbackStream::isStreamReady() const
{
    return m_isStreamReady;
}

bool LoopbackStream::isPlayingContinue() const
{
    return m_isPlayingContinue;
}

void LoopbackStream::setSampleRate(int sampleRate)
{
    if (sampleRate < 16000)
        return;
    m_sampleRate = sampleRate;
}

void LoopbackStream::setFramesPerBuffer(int framesPerBuffer)
{
    // Update number of frames per buffer.
    if (framesPerBuffer <= 0)
        return;
    m_streamFramePerBuffer = framesPerBuffer;
    m_inputBufferSize = m_streamFramePerBuffer * m_sizePerSample * m_channelsCount;
}

#ifdef WIN32
void LoopbackStream::setInputLatency(double inputLatency)
{
    if (inputLatency > 0.0)
        m_inputLatency = inputLatency;
}

void LoopbackStream::setOutputLatency(double outputLatency)
{
    if (outputLatency > 0.0)
        m_outputLatency = outputLatency;
}
#elif __linux__
void LoopbackStream::usePortAudio(bool value)
{
    m_usePortAudio = value;
}
#endif

void LoopbackStream::setBackend(BackendAudio backend)
{
    bool isValid = false;
    for (BackendAudio availableBackend : m_availableBackend)
    {
        if (backend == availableBackend)
        {
            isValid = true;
            break;
        }
    }

    if (isValid && backend != BackendAudio::SYSTEM_DEFAULT)
        m_backend = backend;
    else if (backend == BackendAudio::SYSTEM_DEFAULT)
        m_backend = getDefaultBackend();
    else 
    {
        std::cerr << "Invalid Backend!" << std::endl;
        std::exit(-1);
    }
}
