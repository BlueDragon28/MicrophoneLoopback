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

#include "StreamApplication.h"
#include "CMDParser.h"
#include "Common.h"
#include <portaudio.h>
#include <thread>
#include <chrono>
#ifdef __linux__
#include <csignal>
#endif

// Static pointer to the app initialized.
static StreamApplication* app = nullptr;

StreamApplication::StreamApplication(int& argc, char**& argv) :
    m_stream(nullptr),
    m_isAppContinue(false),
    m_isAppReady(false),
    m_sampleRate(-1),
    m_framesPerBuffer(-1),
#ifdef WIN32
    m_inputLatency(-1.0),
    m_outputLatency(-1.0),
#elif __linux
    m_usePortAudio(false),
#endif
    m_backend(BackendAudio::SYSTEM_DEFAULT)
{
    // Set the app static member to this instance.
    app = this;

    // Parsing command line arguments.
    CMDParser cmdParse(argc, argv);
    if (cmdParse.isSampleRateSet())
        m_sampleRate = cmdParse.sampleRate();
    if (cmdParse.isFramesPerBufferSet())
        m_framesPerBuffer = cmdParse.framesPerBuffer();
#ifdef WIN32
    if (cmdParse.isInputLatencySet())
        m_inputLatency = cmdParse.inputLatency();
    if (cmdParse.isOutputLatencySet())
        m_outputLatency = cmdParse.outputLatency();
#elif __linux__
    m_usePortAudio = cmdParse.usePortAudio();
#endif

    if (cmdParse.isBackendSet())
        m_backend = cmdParse.backend();

    // Initialize PortAudio.
#ifdef __linux__
    if (m_usePortAudio)
    {
#endif
    int err = Pa_Initialize();
    if (err == paNoError)
        m_isAppReady = true;
    else
        m_isAppReady = false;
#ifdef __linux__
    }
#endif

    m_isAppReady = true;

    // Connect the signal handler to catch ctrl-c and terminate signals.
#ifdef WIN32
    createWindowsSignalsCatch();
#elif __linux__
    createSigAction();
#endif
}

StreamApplication::~StreamApplication()
{
    deinit();
}

void StreamApplication::setStream(LoopbackStream* stream)
{
    m_stream = stream;
    if (m_sampleRate > -1)
        m_stream->setSampleRate(m_sampleRate);
    if (m_framesPerBuffer > -1)
        m_stream->setFramesPerBuffer(m_framesPerBuffer);
#ifdef WIN32
    if (m_inputLatency > -1.0)
        m_stream->setInputLatency(m_inputLatency);
    if (m_outputLatency > -1.0)
        m_stream->setOutputLatency(m_outputLatency);

    m_stream->setBackend(m_backend);
#elif __linux__
    m_stream->usePortAudio(m_usePortAudio);
    if (m_usePortAudio)
    {
        m_stream->setBackend(m_backend);
    }
#endif
    m_stream->init();
}

bool StreamApplication::isAppReady() const
{
    if (!m_stream) return false;
    return m_isAppReady && m_stream->isStreamReady();
}

bool StreamApplication::isAppContinue() const
{
    return m_isAppContinue;
}

int StreamApplication::run()
{
    if (!m_stream) return EXIT_FAILURE;

    if (!isAppReady()) return EXIT_FAILURE;

    // Check if the stream is initialized before lauching the main loop.
    if (!m_stream->isStreamReady())
        m_stream->init();
    if (!m_stream->isPlayingContinue())
        m_isAppContinue = m_stream->play();
    else
        m_isAppContinue = false;

    if (!m_isAppContinue) return EXIT_FAILURE;

    // Main loop of the program.
    while (m_isAppContinue && m_stream->isPlayingContinue())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        if (m_isAppContinue)
            m_isAppContinue = m_stream->isPlayingContinue();
    }

    return EXIT_SUCCESS;
}

void StreamApplication::stopApplication()
{
    m_isAppContinue = false;
    deinit();
}

void StreamApplication::deinit()
{
    m_stream->deinit();
#ifdef __linux__
    if (m_usePortAudio)
#endif
    Pa_Terminate();
}

#ifdef WIN32
void StreamApplication::createWindowsSignalsCatch()
{
    SetConsoleCtrlHandler(StreamApplication::windowsSignalsHandler, TRUE);
}

BOOL StreamApplication::windowsSignalsHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT 
        || signal == CTRL_CLOSE_EVENT 
        || signal == CTRL_LOGOFF_EVENT
        || signal == CTRL_SHUTDOWN_EVENT
        || signal == CTRL_BREAK_EVENT)
    {
        app->stopApplication();
    }
    return TRUE;
}
#elif __linux__
void StreamApplication::createSigAction()
{
    // Connect the signal handler to catch ctrl-c signals.
    struct sigaction sigHandler = {};
    sigHandler.sa_handler = StreamApplication::sigActionHandler;
    sigemptyset(&sigHandler.sa_mask);
    sigHandler.sa_flags = 0;
    sigaction(SIGINT, &sigHandler, NULL);
    sigaction(SIGTERM, &sigHandler, NULL);
}

void StreamApplication::sigActionHandler(int signal)
{
    // If the signal ctrl-c or terminate is catch, stopping the application.
    if (signal == SIGINT || signal == SIGTERM)
    {
        if (app)
        {
            app->stopApplication();
        }
    }
}
#endif
