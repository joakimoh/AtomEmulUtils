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
  
