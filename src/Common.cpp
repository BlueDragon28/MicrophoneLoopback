#include "Common.h"
#include <portaudio.h>

bool validateBackend(BackendAudio backend)
{
    if (
        backend == BackendAudio::SYSTEM_DEFAULT ||
        backend == BackendAudio::DIRECT_SOUND ||
        backend == BackendAudio::MME ||
        backend == BackendAudio::ASIO ||
        backend == BackendAudio::WASAPI ||
        backend == BackendAudio::WDMKS ||
        backend == BackendAudio::OSS ||
        backend == BackendAudio::ALSA ||
        backend == BackendAudio::JACK)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

BackendAudio fromHostApiToBackend(PaHostApiTypeId id)
{
    switch (id)
    {
    case paDirectSound:
        return BackendAudio::DIRECT_SOUND;
    case paMME:
        return BackendAudio::MME;
    case paASIO:
        return BackendAudio::ASIO;
    case paWASAPI:
        return BackendAudio::WASAPI;
    case paWDMKS:
        return BackendAudio::WDMKS;
    case paOSS:
        return BackendAudio::OSS;
    case paALSA:
        return BackendAudio::ALSA;
    case paJACK:
        return BackendAudio::JACK;
    default:
        return BackendAudio::SYSTEM_DEFAULT;
    }
}

std::vector<BackendAudio> getAvailablesBackend()
{
    PaHostApiIndex hostApiCount = Pa_GetHostApiCount();

    if (hostApiCount <= 0)
    {
        return std::vector<BackendAudio>();
    }

    std::vector<BackendAudio> availableBackend;
    availableBackend.push_back(BackendAudio::SYSTEM_DEFAULT);

    for (int i = 0; i < hostApiCount; i++)
    {
        const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(i);
        const PaHostApiTypeId hostApiID = hostApiInfo->type;
        availableBackend.push_back(fromHostApiToBackend(hostApiID));
    }

    return availableBackend;
}

BackendAudio getDefaultBackend()
{
    return fromHostApiToBackend(Pa_GetHostApiInfo(Pa_GetDefaultHostApi())->type);
}

