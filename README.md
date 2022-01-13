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

# Licence

See the LICENCE file.