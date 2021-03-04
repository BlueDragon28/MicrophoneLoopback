#include "StreamApplication.h"
#include <iostream>
#include <portaudio.h>
#include <thread>
#include <chrono>
#include <csignal>

// Static pointer to the app initialized.
static StreamApplication* app = nullptr;

StreamApplication::StreamApplication() :
    m_stream(nullptr),
    m_isAppContinue(false),
    m_isAppReady(false)
{
    // Set the app static member to this instance.
    app = this;

    // Initialize PortAudio.
    int err = Pa_Initialize();
    if (err == paNoError)
        m_isAppReady = true;
    else
        m_isAppReady = false;

    // Connect the signal handler to catch ctrl-c and terminate signals.
#ifdef WIN32
#elif __linux__
    createSigAction();
#endif
}

StreamApplication::~StreamApplication()
{
    m_stream->deinit();
    Pa_Terminate();
}

void StreamApplication::setStream(LoopbackStream* stream)
{
    m_stream = stream;
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

    if (!m_stream->isPlayingContinue())
        m_isAppContinue = m_stream->play();
    else
        m_isAppContinue = false;
    if (!m_isAppContinue) return EXIT_FAILURE;

    while (m_isAppContinue && m_stream->isPlayingContinue())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));

        if (m_isAppContinue)
            m_isAppContinue = m_stream->isPlayingContinue();
    }

    return EXIT_SUCCESS;
}

void StreamApplication::stopApplication()
{
    m_isAppContinue = false;
}

#ifdef WIN32
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