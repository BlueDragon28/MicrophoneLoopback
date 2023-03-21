#include "Common.h"
#include <portaudio.h>
#include <iostream>

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

std::string fromBackendToString(BackendAudio backend)
{
    switch (backend)
    {
    case BackendAudio::SYSTEM_DEFAULT:
        return "System Default";
    case BackendAudio::DIRECT_SOUND:
        return "DirectSound";
    case BackendAudio::MME:
        return "MME";
    case BackendAudio::ASIO:
        return "ASIO";
    case BackendAudio::WASAPI:
        return "WASAPI";
    case BackendAudio::WDMKS:
        return "WDMKS";
    case BackendAudio::OSS:
        return "OSS";
    case BackendAudio::ALSA:
        return "ALSA";
    case BackendAudio::JACK:
        return "JACK";
    default:
        return "Invalid API";
    }
}

void displayAvailableBackend()
{
    if (Pa_Initialize() != paNoError)
    {
        std::cerr << "Failed to initialize PortAudio!" << std::endl;
        std::exit(-1);
    }

    const std::vector<BackendAudio> availableBackend = getAvailablesBackend();

    std::cout << "List of available backends for PortAudio" << '\n'
        << "ID:NAME" << '\n';

    for (const BackendAudio backend : availableBackend)
    {
        std::cout << (int)backend << ":" << fromBackendToString(backend) << "\n";
    }

    if (Pa_Terminate() != paNoError)
    {
        std::cerr << "Failed to deinit PortAudio!" << std::endl;
        std::exit(-1);
    }
}

int fromBackendToHostApiID(BackendAudio backend)
{
    switch (backend) 
    {
        case BackendAudio::DIRECT_SOUND:
            return paDirectSound;
        case BackendAudio::MME:
            return paMME;
        case BackendAudio::ASIO:
            return paASIO;
        case BackendAudio::WASAPI:
            return paWASAPI;
        case BackendAudio::WDMKS:
            return paWDMKS;
        case BackendAudio::OSS:
            return paOSS;
        case BackendAudio::ALSA:
            return paALSA;
        case BackendAudio::JACK:
            return paJACK;
        default:
            return -1;
    }
}
