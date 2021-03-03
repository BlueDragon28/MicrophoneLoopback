#include <iostream>
#include "StreamApplication.h"
#include "LoopbackStream.h"

int main()
{
    StreamApplication app;

    LoopbackStream stream;
    stream.init();

    app.setStream(&stream);

    bool result = app.run();

    return result;
}