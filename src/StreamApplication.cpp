#include "StreamApplication.h"
#ifdef WIN32
#include <portaudio.h>
#endif
#include <thread>
#include <chrono>
#ifdef __linux__
#include <csignal>
#endif

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
#ifdef WIN32
    int err = Pa_Initialize();
    if (err == paNoError)
        m_isAppReady = true;
    else
        m_isAppReady = false;
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
#ifdef WIN32
    Pa_Terminate();
#endif
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