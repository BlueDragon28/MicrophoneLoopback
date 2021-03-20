# MicrophoneLoopback

**MicrophoneLoopback** is a little program to loopback the microphone sound into the speakers. It's working on both Windows and Linux.

# Installation

## Windows

**MicrophoneLoopback** use the [PortAudio API](http://www.portaudio.com/) to capture the microphone stream and send it back to the speakers.  
To compile MicrophoneLoopback, you first need to compile PortAudio, follow this [PortAudio installation guide](http://files.portaudio.com/docs/v19-doxydocs/tutorial_start.html).

The **PortAudio binary** files must be in the **dependencies/portaudio** folders with :
- **lib**: put in this folder the file **portaudio_static_x64.lib** file.
- **include**: put the **PortAudio** include file.

Then, compile the **MicrophoneLoopback** with [CMake](https://cmake.org/).

## Linux

On **Linux**, **MicrophoneLooback** use the **pulse_simple api** to capture microphone stream and send it back to the speakers.

To compile **MicrophoneLoopback** you need to have pulseaudio installed on your system.

# Licence

See the LICENCE file.