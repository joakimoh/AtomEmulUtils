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

### Scheduling
All the devices are usually 'stepped' one by one in rounds.
The microcontroller can be stepped a complete instruction (NMOS 6502 Instruction-stepped) or a micro cycle (NMOS 6502 Micro cycle-stepped)
at a time.
In each round (that starts with stepping the microprocessor), each other device will be stepped as many steps it takes to
match the time of the microprocessor. 
Stepping is made in 'ticks' where one tick defaults to one microprocessor micro cycle unless specfied to be different.
'tick time' is synchronised with real time on a low rate basis (EMU_LOW_RATE), ususally 50-60 Hz
(default is derived from the frame rate of the TV system standard chosen but can also be specified to be different).
It is also possible to specify that a device shall be stepped at a different rate than the microprocessor step rate (one to a few micro cycles).
This rate could be the low rate (EMU_LOW_RATE) or the high rate (EMU_HIGH_RATE) specified by configuration (usually a rate corresponding to a half TV scan line is used, e.g., 31 250 Hz).
The low rate is suitable for keyboard devices whereas the high rate works well for sound devices.

### Interaction between devices

#### Ports
Devices have ports that correspond to pins on the real-life components. A memory device (like a DRAM or a ROM) has a chip select (CS) port and an
8-bit latch has data outputs Q0 to Q7 e.g. Not all real-life pins are available as ports though - the data and address bus pins of memory-mapped devices and
the microprocessor as well as clock pins are in general not represented as ports. The reason for the latter is that it would be too time-consuming to emulate
data, address and clock pins transitions. See [Memory accesses and clocking](#memory-accesses-and-clocking) below.

Pins are also grouped into a single port for pins that are strongly couple, e.g., the output pins Q0 to Q7 of the 8-bit latch is represented as a 8-bit single port. Pins are either
of type INPUT, OUTPUT or BI-DIRECTIONAL. A port is digital and can have up to 16 bits. (There are also analogue ports, see [Analogue ports](analogue-ports) below.)

Each device registers its ports at initialisation (start up) to make them known. Below is shown a couple of one-bit ports registered by the 6502 microprocessor:
```
	registerPort("RDY", IN_PORT, 0x01, RDY, &mRDY);
	registerPort("SYNC", OUT_PORT, 0x01, SYNC, &mSYNC);
```
(_"RDY"_ is the name of the port to be referenced in the configration file, _RDY_ is a unique port index, _0x1_ is a mask specifying the port's bit-size and _mRDY_ is the variable holding the value of the port.)

In the configuration file (a k a map file) that specifies the emulated computer system, a port of one device can be connected to a port of another device.
It is possible to specify that only a few bits of one device's port shall be connected to a few bits of another device's port.
Below is shown an example where the device _VDU_'s port _FS_ is connected to bit 7 of the device PIA's port _PortC_:
```
CONNECT		VDU:FS			PIA:PortC;7			
```

At run time, each device will inform that an (output or bi-directional) port is updated. Below shows how a VDU device informs that the horizontal sync (HS)
port has a new value (0):
```
updatePort(HS, 0);
```
The emulation engine will act upon this and inform all connected receiving devices (as specified by the configuration file _CONNECT_ statements) by updating the value of the receiving device's input port.
The receving device will then always have an updated value of its connected input ports.

#### Memory accesses and clocking
As mentioned above, data and address bus pins of memory-mapped devices are in general not represented as ports. Instead each memory-mapped device that can be accessed by the microprocessor
defines a read/write interface which can be called by the microprocessor:
```
virtual bool writeByte(BusAddress adr, BusByte data);
virtual bool readByte(BusAddress adr, BusByte& data);
```
A memory map (actually a balanced binary tree) maintained by the emulator engine will route all memory accesses made by the microprocessor to the correct device's read/write
interface. The memory space occupied by one device is specified when the decice is 'added' to the system in the configuration file. Below shows the addition of a DRAM memory
labeled 'RAM32K' which occupies the memory space 0x000 to 0x7fff. (The '4' specifies the access rate supported by the memory in MHz.)
```
ADD	DRAM			RAM32K		0000	8000	4						// 32kB RAM	0000 -	7fff
```

Video devices will use the read/write interface of memory devies (DRAM or RAM) in a similar way as the microprocessor but there is usually no routing to be made as
the memory device used by a video device is identified when processing the configuration file. Below is shown how the device _VDU' (of device type _VDU6847_)
is specified to occupy the memory space 0x800 to 0x8ff (for access of  its internal registers) while accessing video RAM located at address 0x8000.
```
ADD	VDU6847		VDU			0800	0100		1			8000					// VDU CRT Controller 6847
```

The clock port of a device (if it is a clocked device) doesn't have to be represented by a port simply becasue the 'clocking' is implemented by the scheduling described above ([Scheduling](#scheduling)).

#### Analogue ports

## Headless operation vs display operation
If no video display device (either a MC6847 or a BBC Micro Video ULA) is specified, the emulator will run in 'headless' mode. In headless mode the only way to
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