# AtomEmulUtils
AtomEmulUtils is an emulation framework that makes it possible to "build" a 6502-based computer system from a set of predefined hardware devices.
Devices are selected and "connected" using a configuration file (referred to as a map file as it includes a memory map for the emulated system).
The following hardware devices are currently supported:
- ADC 7002 12-bit Analogue-to-Digital Converter (used in e.g., the BBC Micro)
- DAC ZN428 8-bit Digital-to-Analogue Converter
- M6850 ACIA
- Acorn Atom Cassette Interface
- Acorn Atom Keyboard (the keyboard matrix)
- Acorn Atom Sound Device (basically just a speaker)
- BBC Micro Keyboard (the keyboard matrix)
- BBC Micro Paged Memory Selection Interface
- BBC Micro Serial ULA 
- BBC Micro Video ULA
- H6845 CRT Controller
- 74LS259 8-bit Addressable Latch
- 8255 PIA
- NMOS 6502 Instruction-stepped
- NMOS 6502 Micro cycle-stepped
- DRAM
- Static RAM
- ROM (a ROM data file associated with it can hold e.g., an operating system)
- SN76489 Tone Generator (used in e.g., the BBC Micro)
- SAA5050 Teletext Character Generator
- MC6847 Video Display Generator
- 6522 VIA

There are also a few devices that are not part of the computer system that emulates external equipment connected to the computer system:
- SD Card with SPI interface (SD Card File System ROM software like https://github.com/hoglet67/MMFS for the BBC Micro can use this)
- Tape Recorder (allows for tape audio files to be streamed to and from the computer system)

See [Installation instructions](docs/Installation.md) for details on how to install the emulator.

See [Implementation of the emulator](docs/Implementation.md) for details on how the emulator works and is implemented.

The emulator is run from command line and cannot run without a map file that defines your computer system:
```
> Emu6502 -map <path to map file>
```
You will find several example systems in the folder 'Examples'.
E.g., 'AtomEmulUtils/Examples/Atom/ROMs/AtomMemoryMap.txt' defines a basic Acorn Atom computer system
To start the emulation of that system you simply give the command:
```
> Emu6502 -map AtomEmulUtils/Examples/Atom/ROMs/AtomMemoryMap.txt
```
Other systems include a BBC Micro system and some simple headless systems (systems just with a microprocessor and memory).

The map file and ROM files (with operating systems or your own software) need to be located in the same folder as the
map file.

The utility deson't include any assembler but the example assembler files are written in a way that is compatible with e.g.,
the beebasm assmebler (https://github.com/stardot/beebasm). What is included is however a simple disassmebler ('Dis') that
you can run on any binary file content (like a ROM file).

Details about the content of a map file cand be found in [Map File Content](docs/Configuration.md).
See [Menus](docs/Menu.md) for details about all the menu options.

## Connectivity

### Keyboard
If a keyboard device is specified, then keyboard presses will be forwarded to that device. Is is also possible to 'bypass' keyboard presses and 'paste' text
from the host computer's clipboard directly to the keyboard device.

### Audio
If a sound device is specified, then sound produced by it will be encoded as an audio stream and played on the host computer.

### Tape recorder
If a tape recorder is specified, then tape audio files (CSW format) can be be 'played' to generate a bit stream to the emulated computer system
and bit streams from the emulated computer system can 'recorded' into tape audio files (CSW format).

### Display
If a video display device is specified, a display with a resolution matching that of the TV standard supported by the device will be created. This display
will then be updated by the video display device based on the video memory content.
If no video display device (either a MC6847 or a BBC Micro Video ULA) is specified, the emulator will run in 'headless' mode. In headless mode the only way to
interact with the emulated computer system is via the debugger interface (see below). 

### SD Card
If the SD Card device is specified, then the emulated computer system can interact with it over SPI. Inserting and ejected the card is emulated by
a menu that selects an actual SD Card file.

## Debugger
A command-line debugger can be started at any time and will make it possible to get and change the status of the emulated computer system.
The debugger monitor will be the terminal from which the emulator was started from. See [Debugger commands](docs/Debugger.md)

## Misc.

### Loading/saving files to/from memory
It is possible to load data into the emulated memory directly by selecting a file with binary data. Memory data can also be saved to file.
This makes it possible to quickly load e.g. a BASIC program into memory without having to use the slow cassette interface.

### Emulation speed
The emulation speed is default 'real time' but can be changed to be slower or faster. How fast it can become depends on the host computer and the complexity of
the emauled computer system. Don't expect much higgher speed than real time for a complex system. When a video display device is present, the update rate 
(perceived frame rate) of the display can be adjusted (slowed down) to improvde emulation performance. The default is to use that of the selected TV standard
(50 Hz for PAL and 60 Hz for NTSC).
