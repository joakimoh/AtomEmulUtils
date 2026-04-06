# AtomEmulUtils
AtomEmulUtils is an emulation framework that makes it possible to "build" a 6502-based computer system from a set of predefined hardware devices. Devices are selected and "connected" using a configuration file.
The following hardware devices are currently supported:
- ADC 7002 12-bit Analogue-to-Digital Converter (used in e.g., the BBC Micro)
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
- TI4689 Tone Generator (used in e.g., the BBC Micro)
- SAA5050 Teletext Character Generator
- MC6847 Video Display Generator
- 6522 VIA

There are also a few devices that are not part of the computer system that emulates external equipment connected to the computer system:
- SD Card with SPI interface (SD Card File System ROM software like https://github.com/hoglet67/MMFS for the BBC Micro can use this)
- Tape Recorder (allows for tape audio files to be streamed to and from the computer system)
  
## How the emulator works
All the devices are usually 'stepped' one by one in rounds.
The microcontroller can be stepped a complete instruction (NMOS 6502 Instruction-stepped) or a micro cycle (NMOS 6502 Micro cycle-stepped)
at a time.
In each round (that starts with stepping the microprocessor - either  a complete instruciton or one micro cycle), each other device will be stepped as many steps it takes to
match the time of the microprocessor. 
Stepping is made in 'ticks' where one tick defaults to one microprocessor micro cycle unless specfied to be different.
'tick time' is synchronised with real time usually on a low rate basis (EMU_LOW_RATE), ususally 50-60 Hz
(default is derived from the frame rate of the TV system standard chosen but can also be specified to be different).
It is also possible to specify that a device shall be stepped at a different rate than the microprocessor step rate (one to a few micro cycles).
This rate could be the low rate (50-60 Hz as specified above) or a high rate (EMU_HIGH_RATE) specified by configuration (usually a rate corresponding to a half TV scan line is used, e.g., 31 250 Hz).
The low rate is sutiable fopr keyboard devices wheras the high rate works well for sound devices.

## Headless operation vs display operation
If no video display device (either a MC6847 or a BBC Micro Video ULA) is specied, the emulator will run in 'headless' mode. In headless mode the only way to
interact with the emulated computer system is via the debugger interface (see below). If a video display device is specified, a display with a resolution
matching that of the TV standard supported by the device will be shown.

## Keyboard operation
If a keyboard device is specified, then keyboard presses will be forwarded to that device. Is is also possible to 'bypass' keyboard presses and 'paste' text
from the host computer's clipboard directly to the keyboard device.

## Audio operation
If a sound device is specified, then sound produced by it will be forwarded to the host computer.

## Tape recorder
If a tape recorder is specified, then tape audio files (CSW format) can be be 'played' to generate a bit stream to the emulated computer system
and bit streams from the emulated computer system can 'recorded' into tape audio files (CSW format).

## Debugger
A command-line debugger can be started at any time and will make it possible to get and change the status of the emulated computer system.

## SD Card operation
If the SD Card device is specified, then the emulated computer system can interact with it over SPI. It is possible to insert and eject
an SD Card.

## Loading/saving files to/from memory
It is possible to load data into the emulated memory directly by selecting a file with binary data. Memory data can also be saved to file.
This makes it possible to quickly load e.g. a BASIC program into memory without having to use the slow cassette interface.