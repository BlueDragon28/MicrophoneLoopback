# MicrophoneLoopback

**MicrophoneLoopback** is a little program to loopback the microphone sound into the speakers. It's working on both Windows and Linux.

# Installation

## Windows

**MicrophoneLoopback** use the [PortAudio API](http://www.portaudio.com/) to capture the microphone stream and send it back to the speakers. [cxxopts](https://github.com/jarro2783/cxxopts) to parse commands lines arguments and [ini_parser](https://github.com/BlueDragon28/ini_parser) to parse **ini** file.

To compile MicrophoneLoopback, you first need to compile PortAudio, follow this [PortAudio installation guide](http://files.portaudio.com/docs/v19-doxydocs/tutorial_start.html).

The **PortAudio binary** files must be in the **dependencies/portaudio** folders with :
- **lib**: put in this folder the file **portaudio_static_x64.lib** file.
- **include**: put the **PortAudio** include file.

Clone the **cxxopts** library and the **ini_parser** library into the **dependencies** folder.

Then, compile the **MicrophoneLoopback** with [CMake](https://cmake.org/).

## Linux

On **Linux**, **MicrophoneLooback** use the **pulse_simple api** or the [PortAudio](https://github.com/PortAudio/portaudio) to capture microphone stream and send it back to the speakers, [cxxopts](https://github.com/jarro2783/cxxopts) for parsing commands lines arguments and [ini_parser](https://github.com/BlueDragon28/ini_parser) for parsing **ini** file.

To compile **MicrophoneLoopback** you need to have **pulseaudio**, [PortAudio](https://github.com/PortAudio/portaudio), [cxxopts](https://github.com/jarro2783/cxxopts) and [ini_parser](https://github.com/BlueDragon28/ini_parser) installed on your system.

# How to use

**MicrophoneLoopback [OPTION...]**

- **-r, --sample-rate arg** : Set the sample rate at which the program will loopback the sound of the microphone to the speakers. The default value is **48000**Hz.
- **-f, --frames-per-buffer arg** : Set the number of frames per buffer, a lower value will decrease the latency, but will increase cpu overhead and glitches. The default value is **256**.
- **-s, --short arg** : Allow to use short version for setting the sample rate :
  - **44 -> 44100**
  - **48 -> 48000**
  - **96 -> 96000**

  This settings is overridden by **--sample-rate**.
- **-v, --version** : show the version of the program.
- **-h, --help** : show a help text on the available options of the program.

## Windows specific

- **-i, --input_latency arg** : Set the input latency in seconds at which Windows will get the audio from the microphone. The default value is **0.02**.
- **-o, --output_latency arg** : Set the output latency in seconds at which Windows will send the audio to the speakers. The default value is **0.02**.

## Linux specific

- **-p, --portaudio** : Use PortAudio API instead of the Pulse Simple API.

## Configuration

It is possible to configure MicrophoneLoopback with a **.conf** file. An exemple template [here](https://github.com/BlueDragon28/MicrophoneLoopback/blob/development/MicrophoneLoopback.conf). The file use an **ini** syntax.

Ini file :

``` ini
# Configuration file to configure MicrophoneLoopback.

[stream]
#sample-rate=48000
#frames-per-buffer=256

[Windows]
#input_latency=0.02
#output_latency=0.02

[api]
#use-portaudio=yes
```

On Windows the file must be put in the same location of the executable. On Linux, the file may be put either in `/home/user/.config/MicrophoneLoopback/` or in `/etc/MicrophoneLoopback`.

# Licence

See the LICENCE file.