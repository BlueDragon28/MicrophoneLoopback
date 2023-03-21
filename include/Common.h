#ifndef STREAMAPPLICATION_Common
#define STREAMAPPLICATION_Common

#include <vector>

enum class BackendAudio
{
    INVALID_API = -1,
    SYSTEM_DEFAULT,
    DIRECT_SOUND,
    MME,
    ASIO,
    WASAPI,
    WDMKS,
    OSS,
    ALSA,
    JACK
};

std::vector<BackendAudio> getAvailablesBackend();
BackendAudio getDefaultBackend();

#endif // STREAMAPPLICATION_Common
