/*
 * MIT Licence
 * 
 * MicrophoneLoopback
 * 
 * Copyright © 2021 Erwan Saclier de la Bâtie (Erwan28250)
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

CMDParser::CMDParser(int& argc, char**& argv) :
    m_isSampleRateSet(false),
    m_sampleRate(0),
    m_isframesPerBufferSet(false),
    m_framesPerBuffer(0),
#ifdef WIN32
    m_isInputLatencySet(false),
    m_inputLatency(-1.0),
    m_isOutputLatencySet(false),
    m_outputLatency(-1.0)
#elif __linux__
    m_usePortAudio(false)
#endif
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
        ("v,version", "Show the version of the program.")
        ("h,help", "Print usage information.");
    
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
#elif __linux__
    // Use PortAudio
    m_usePortAudio = result["portaudio"].as<bool>();
#endif
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
