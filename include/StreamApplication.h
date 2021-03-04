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
    StreamApplication();
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
};

#endif // STREAMAPPLICATION_MLB_H