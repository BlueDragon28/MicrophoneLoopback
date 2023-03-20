#ifndef STREAMAPPLICATION_Common
#define STREAMAPPLICATION_Common

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

#endif // STREAMAPPLICATION_Common
