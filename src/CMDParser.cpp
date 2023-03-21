/*
 * MIT Licence
 * 
 * MicrophoneLoopback
 * 
 * Copyright © 2022 Erwan Saclier de la Bâtie (BlueDragon28)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "CMDParser.h"
#include "Common.h"
#include <cstdlib>
#include <ini_parser.h>
#include <cmath>
#include <iostream>

#ifdef __linux__
#include <pwd.h>
#include <unistd.h>
#endif

CMDParser::CMDParser(int& argc, char**& argv) :
    m_isSampleRateSet(false),
    m_sampleRate(0),
    m_isframesPerBufferSet(false),
    m_framesPerBuffer(0),
#ifdef WIN32
    m_isInputLatencySet(false),
    m_inputLatency(-1.0),
    m_isOutputLatencySet(false),
    m_outputLatency(-1.0),
#elif __linux__
    m_usePortAudio(false),
#endif
    m_isBackendSet(false),
    m_backend(BackendAudio::SYSTEM_DEFAULT)
{
    // Parsing command line arguments.
    if (argc <= 0 || argv == nullptr)
        return;
    
    // cxxopts object used to parse command line arguments.
    cxxopts::Options options("MicrophoneLoopback", "Stream microphone sound to speakers with low latency.");

    options.add_options()
        ("r,sample-rate", "Sample rate (default: 48000)\nDo not go above devices maximum supported values.", cxxopts::value<int>())
        ("s,short", "Short version for sample rate: 44 (44100), 48 (48000), 96 (96000). Overridden by sample-rate.", cxxopts::value<int>())
        ("f,frames-per-buffer", 
            "Number of frames per buffer (default: 256). A lower value will get a better latency but more cpu overhead and glitches.",
            cxxopts::value<int>())
#ifdef WIN32
        ("i,input_latency", "Latency in seconds at which Windows will try to operate to get audio from the microphone (default: 0.02).", cxxopts::value<double>())
        ("o,output_latency", "Latency in seconds at which Windows will try to operate to send audio to the dac (default: 0.02).", cxxopts::value<double>())
#elif __linux__
        ("p,portaudio", "Use PortAudio API instead of the Pulse Simple API.", cxxopts::value<bool>()->default_value("false"))
#endif
        ("b,backend-audio", "The id of the backend audio to use. Only for PortAudio!!!", cxxopts::value<int>())
        ("a,available-backends", "Display the available backend name along side their ID")
        ("v,version", "Show the version of the program.")
        ("h,help", "Print usage information.");
    
    // ini_parser object used to read ini file.
    ini_parser ini;
    bool isValid = false;
    std::string fIniPath;
#ifdef WIN32
    fIniPath = "./MicrophoneLoopback.conf";
    ini.setIniFile(fIniPath, true);
	if (ini.isParsed())
		std::cout << fIniPath << std::endl;
#elif __linux__
    const char* homeDir = getenv("HOME");
    if (homeDir == nullptr)
        homeDir = getpwuid(getuid())->pw_dir;
    fIniPath = homeDir;
    fIniPath += "/.config/MicrophoneLoopback/MicrophoneLoopback.conf";
    ini.setIniFile(fIniPath, true);
    if (!ini.isParsed())
    {
        fIniPath = "/etc/MicrophoneLoopback/MicrophoneLoopback.conf";
        ini.setIniFile(fIniPath, true);
    }
#endif
    
    // Parsing.
    cxxopts::ParseResult result = options.parse(argc, argv);

    // Version
    if (result.count("version"))
    {
        std::cout << "v0.2" << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    // Help
    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    // Short
    if (result.count("short"))
    {
        int shortRate = result["short"].as<int>();
        int sampleRate = 0;

        if (shortRate == 44)
            sampleRate = 44100;
        else if (shortRate == 48)
            sampleRate = 48000;
        else if (shortRate == 96)
            sampleRate = 96000;
        
        if (sampleRate > 0)
        {
            m_sampleRate = sampleRate;
            m_isSampleRateSet = true;
        }
        else
        {
            std::cout << "Short sample rate wrong arguments. Possible value are 44, 48 and 96." << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    // Sample rate
    if (result.count("sample-rate"))
    {
        m_sampleRate = result["sample-rate"].as<int>();
        if (m_sampleRate < 16000)
        {
            std::cout << "Sample rate cannot be below 16000." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        m_isSampleRateSet = true;
    }
    else if (ini.isParsed())
    {
        std::string sSampleRate = ini.getValue("stream", "sample-rate", &isValid);
        if (isValid)
        {
            try
            {
                int sampleRate = std::stoi(sSampleRate);
                if (sampleRate < 16000)
                {
                    std::cout << "Ini error: sample rate cannot be below 16000." << std::endl;
                    std::exit(EXIT_FAILURE);
                }
                m_sampleRate = sampleRate;
                m_isSampleRateSet = true;
            }
            catch (...)
            {
                std::cout << "Ini error: sample rate must be a integer." << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
    }

    // Frames per buffer.
    if (result.count("frames-per-buffer"))
    {
        m_framesPerBuffer = result["frames-per-buffer"].as<int>();
        if (m_framesPerBuffer <= 0)
        {
            std::cout << "Frames per buffer must be higher than 0." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        m_isframesPerBufferSet = true;
    }
    else if (ini.isParsed())
    {
        std::string sFramesPerBufer = ini.getValue("stream", "frames-per-buffer", &isValid);
        if (isValid)
        {
            try
            {
                int framesPerBuffer = std::stoi(sFramesPerBufer);
                if (framesPerBuffer <= 0)
                {
                    std::cout << "Ini error: frames per buffer must be higher than 0." << std::endl;
                    std::exit(EXIT_FAILURE);
                }
                m_framesPerBuffer = framesPerBuffer;
                m_isframesPerBufferSet = true;
            }
            catch (...)
            {
                std::cout << "Ini error: frames per buffer must be an integer." << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
    }

#ifdef WIN32
    // Input latency
    if (result.count("input_latency"))
    {
        m_inputLatency = result["input_latency"].as<double>();
        if (m_inputLatency <= 0.0)
        {
            std::cout << "Latency must be highter than 0." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        m_isInputLatencySet = true;
    }
    else if (ini.isParsed())
    {
        std::string sInputLatency = ini.getValue("Windows", "input_latency", &isValid);
        if (isValid)
        {
            try
            {
                double inputLatency = std::stod(sInputLatency);
                if (inputLatency <= 0.0)
                {
                    std::cout << "Ini error: input latency must be highter than 0." << std::endl;
                    std::exit(EXIT_FAILURE);
                }
                m_inputLatency = inputLatency;
                m_isInputLatencySet = true;
            }
            catch (...)
            {
                std::cout << "Ini error: input latency must be a valid floating point number." << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
    }
    
    // Output latency
    if (result.count("output_latency"))
    {
        m_outputLatency = result["output_latency"].as<double>();
        if (m_outputLatency <= 0.0)
        {
            std::cout << "Latency must be highter than 0." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        m_isOutputLatencySet = true;
    }
    else if (ini.isParsed())
    {
        std::string sOutputLatency = ini.getValue("Windows", "output_latency", &isValid);
        if (isValid)
        {
            try
            {
                double outputLatency = std::stod(sOutputLatency);
                if (outputLatency <= 0.0)
                {
                    std::cout << "ini error: output latency must be highter than 0." << std::endl;
                    std::exit(EXIT_FAILURE);
                }
                m_outputLatency = outputLatency;
                m_isOutputLatencySet = true;
            }
            catch (...)
            {
                std::cout << "Ini error: output latency must be a valid floating point number." << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
    }
#elif __linux__
    // Use PortAudio
    m_usePortAudio = result["portaudio"].as<bool>();
    if (!m_usePortAudio && ini.isParsed())
    {
        std::string sUsePortAudio = ini.getValue("api", "use-portaudio", &isValid);
        if (isValid)
        {
            if (sUsePortAudio == "yes" ||
                sUsePortAudio == "on" ||
                sUsePortAudio == "true" ||
                sUsePortAudio == "1")
                m_usePortAudio = true;
        }
    }
#endif

    if (result.count("backend-audio"))
    {
        const BackendAudio backend = (BackendAudio)result["backend-audio"].as<int>();

        if (!validateBackend(backend))
        {
            std::cerr << "Invalid Backend!" << std::endl;
            std::exit(-1);
        }

        m_isBackendSet = true;
        m_backend = backend;
    } else if (ini.isParsed())
    {
        std::string sBackend = ini.getValue("stream", "backend-audio", &isValid);

        if (isValid)
        {
            try 
            {
                BackendAudio backend = (BackendAudio)std::stoi(sBackend);

                if (!validateBackend(backend))
                {
                    std::cerr << "Invalid Backend!" << std::endl;
                    std::exit(-1);
                }

                m_backend = backend;
                m_isBackendSet = true;
            } 
            catch (...)
            {
                std::cout << "Ini error: invalid backend number" << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
    }

    if (result.count("available-backends")
#ifdef __linux__
        && m_usePortAudio
#endif
    )
    {
        displayAvailableBackend();
        std::exit(-1);
    }
}

CMDParser::~CMDParser()
{}

bool CMDParser::isSampleRateSet() const
{
    return m_isSampleRateSet;
}

int CMDParser::sampleRate() const
{
    return m_sampleRate;
}

bool CMDParser::isFramesPerBufferSet() const
{
    return m_isframesPerBufferSet;
}

int CMDParser::framesPerBuffer() const
{
    return m_framesPerBuffer;
}

#ifdef WIN32
bool CMDParser::isInputLatencySet() const
{
    return m_isInputLatencySet;
}

double CMDParser::inputLatency() const
{
    return m_inputLatency;
}

bool CMDParser::isOutputLatencySet() const
{
    return m_isOutputLatencySet;
}

double CMDParser::outputLatency() const
{
    return m_outputLatency;
}

#elif __linux__
bool CMDParser::usePortAudio() const
{
    return m_usePortAudio;
}
#endif

bool CMDParser::isBackendSet() const
{
    return m_isBackendSet;
}

BackendAudio CMDParser::backend() const
{
    return m_backend;
}
