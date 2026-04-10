# The configuration (map) file
The map file is the one that defines your computer system. It has different sections (the order of these sections
doesn't matter and you can mix directives as you like in the map file but keeping similar directives in the same
section will easy readability as is therefore recommended).

## The memory mapping section
Here you specify all the devices (including the ones that are not memory-mapped actually).
Each device is added with the directive
```
ADD <device type> <your alias for the device>  <device parameters>
```
Where _device type_ is the selected component (e.g. 'DRAM') and _<your alias for the device_
is a unique name you give that component instance (e.g. 'IC11'). The same type of component
can be be used in several places in the same computer system and each instance then will have a unique
name (e.g. 'IC11' and 'IC12' for two DRAM devices. The name needs also to be unique among all
devices. This is because the name will later be used when defining device ports that shall
be interconnected.

The _device parameters_ will be sligtly different for different types of devices.
A list of exactly  which parameters each device type need to have can be found under
[List of devices and their parameters](#list-of-devices-and-their-parameters).
Below follows a general description of the parameters used for most devices.

Memory-mapped devices have at least the parameters:
```
<start address> <size> <access speed>
```
_start address_ is the lowest address at which the device can be accessed by
the microcontroller (or by a video data unit) and _size_ is the siae in
bytes of the space the device occupies in memory and _access speed_ is the
max speed (in mHz) at which the device can be accessed (this information is
used to decide what type of clock stretching will be required - if enabled).

Devices advance time either based on emulation ticks or the device's
own clock (for some clocked devices). In tha latter case, a clock speed
parameter needs also to be specified.

Example:
```
ADD	VIA6522			SYS_VIA		fe40	0020	1			1.0
```

## List of devices and their parameters
| Device        | Description                           | Memory Access	| Rate Scheduling       | Action on Call        | Immediate Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | --------------------- | ------------------------- | ----------------------------------------------------------------- |
| ADC7002       | 12-bit Analogue-to-Digital Converter  | R/W			| MICROPROCESSOR_RATE	|						|							| \<start address\> \<size> \<access speed\> \<clock speed\>        |
| ZN428         | 8-bit Digital-to-Analogue Converter   | W 			|						|						|							| \<start address\> \<size> \<access speed\> \<clock speed\>		|
| CRTC6845      | H6845 Character Raster Generator      | R/W			|						| Advance one Char		| DEW,GLR,LOSE				| \<start address\> \<size> \<access speed\>						|
| DRAM			| DRAM									| R/W			|						|						|							| \<start address\> \<size> \<access speed\>						|
| SRAM			| Static RAM							| R/W			|						|						|							| \<start address\> \<size> \<access speed\>						|
| ROM			| ROM									| W				|						|						|							| \<start address\> \<size> \<access speed\> \<ROM file\>			|
| SAA5050		| SAA5050 Teletext Character Generator	| R/W			|						| Advance one Char		|							|																	|
| ACIA6850		| M6850 ACIA							| R/W			| MICROPROCESSOR_RATE	| Poll Rx/TX CLK		| CTS,DCD					| \<start address\> \<size> \<access speed\> \<clock speed\>        |
| BeebSerULA	| BBC Micro Serial ULA					| R/W			| MICROPROCESSOR_RATE	|						| TxD,RTSI					| \<start address\> \<size> \<access speed\>						|
| BeebVideoULA	| BBC Micro Video ULA					| R/W			|						|						| RA,VS,HS,C				| \<start address\> \<size> \<access speed\>						|
| BeebRomSel	| BBC Micro Paged Memory Selection		| W				|						|						|							|																	|
| VDU6847		| MC6847 Video Display Generator		| R/W			|						|						|							|																	|
| PIA8255		| 8255 PIA								| R/W			|						|						|							|																	|
| VIA6522		| 6522 VIA								| R/W			|						|						|							|																	|
| CPU_6502		| 6502 Instruction-stepped				|				|						|						|							|																	|
| CPU_6502CC	| 6502 Micro cycle-stepped				|				|						|						|							|																	|
| TAPREC		| Tape Recorder							|				|						|						|							|																	|
| TI4689		| TI4689 Tone Generator					| R/W			|						|						|							|																	|
| SD_CARD		| SD Card with SPI interface			|				|						|						|							|																	|
| ATOMKB		| Acorn Atom Keyboard					|				|						|						|							|																	|
| BEEBKB        | BBC Micro Keyboard				    |				| LOW_RATE (50 Hz)		|						|							| \<keyboard start-up option byte\>									|
| ATOMCAS		| Acorn Atom Cassette Interface			|				|						|						|							|																	|
| ATOMSP		| Acorn Atom Sound Device				|				|						|						|							|																	|
| 74LS259		| 74LS259 8-bit Addressable Latch		|				|						|						|							|																	|