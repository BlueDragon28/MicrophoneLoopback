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

bool validateBackend(BackendAudio backend);
std::vector<BackendAudio> getAvailablesBackend();
BackendAudio getDefaultBackend();
void displayAvailableBackend();
int fromBackendToHostApiID(BackendAudio backend);

#endif // STREAMAPPLICATION_Common
