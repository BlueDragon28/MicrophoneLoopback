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
    m_sampleRate(0)
{
    // Parsing command line arguments.
    if (argc <= 0 || argv == nullptr)
        return;
    
    // cxxopts object used to parse command line arguments.
    cxxopts::Options options("MicrophoneLoopback", "Stream microphone sound to speakers with low latency.");

    options.add_options()
        ("r,sample-rate", "Sample rate (default: 48000)\nDo not go above devices maximum supported values.", cxxopts::value<int>())
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