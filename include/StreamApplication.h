#ifndef STREAMAPPLICATION_MLB_H
#define STREAMAPPLICATION_MLB_H

#include "LoopbackStream.h"

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
    static void signalHandler(int signal);

    LoopbackStream* m_stream;
    bool m_isAppContinue;
    bool m_isAppReady;
};

#endif // STREAMAPPLICATION_MLB_H