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
own clock (for some clocked devices). In the latter case, a clock speed
parameter needs also to be specified.

Example:
```
ADD	VIA6522			SYS_VIA		fe40	0020	1			1.0
```
Video devices can have a parameter that species the start address of the
video memory.
```
<video mem adr>
```

Example:
```
ADD	VDU6847		VDU			0800	0100		1			8000
```

Memory-mapped devices (normally a memory device like a RAM or a ROM) is also allowed
to have small gaps in the memory space they occupy. Such a gap is specified by
the _GAP_ directive.
Example:
```
ADD	ROM				OS			c000	4000	2			OS12.rom	// OS (IC51)		 				c000 - ffff
GAP					OS			fc00	0300							// Gap in OS ROM space between		fc00 - ff00
```

## List of devices and their parameters
The table below lists all devices and specifies whether they are memory-mapped, how they are scheduled, whether immediate prort processing is required and the parameters they need to have.
Scheduling is a combination of rate-based scheduling (column _Rate Scheduling_) and triggering by call from other devices (Columns _At Call_ and _Callees_).
Triggering by call means that another device (the callee) exchanges data with (and sometimes also advances the time of) the called device by explicitly calling a method provided by that device.
The result is that data is exchanged between the two devices and possibly, that time is advanced for the called device. An example of this it that the BeebVideoULA device calls both the SAA5050
and CRTC6845 devices to advance their time correspodning to one screen character. At the same time the BeebVideoULA device will get character data from those devices to use when generatingh 
ideo output. Only the devices listed in the _Callees_ column can be connected to the called device. Another example is that the BeebSerULA communicates changes in the received and transmit
clock speeds to the ACIA6850 device (here  time is not advanced of the called device - only data is exchanged).
The calling is  specified by the TRIGGER directive.

Example:
```
TRIGGER	CRTC		CALL	VDU
```

| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| DRAM			| DRAM									| R/W			|						|					|				|					| \<start address\> \<size> \<access speed\>						|
| SRAM			| Static RAM							| R/W			|						|					|				|					| \<start address\> \<size> \<access speed\>						|
| ROM			| ROM									| W				|						|					|				|					| \<start address\> \<size> \<access speed\> \<ROM file\>			|
| CRTC6845      | H6845 Character Raster Generator      | R/W			| NONE					| Advance one Char	| BeebVideoULA	| DEW,GLR,LOSE		| \<start address\> \<size> \<access speed\>						|
| SAA5050		| SAA5050 Teletext Character Generator	| R/W			| NONE					| Advance one Char	| BeebVideoULA	|					|																	|
| BeebVideoULA	| BBC Micro Video ULA					| R/W			| MICROPROCESSOR_RATE	|					|				| RA,VS,HS,C		| \<start address\> \<size> \<access speed\>						|
| VDU6847		| MC6847 Video Display Generator		| R/W			| MICROPROCESSOR_RATE	|					|				| A/S,GM			| \<start address\> \<size> \<access speed\> \<video mem adr\>		|																		|
| BeebRomSel	| BBC Micro Paged Memory Selection		| W				|						|					|				|					| \<start address\> \<size> \<access speed\>						|
| ACIA6850		| M6850 ACIA							| R/W			| MICROPROCESSOR_RATE	| Poll Rx/TX CLK	| BeebSerULA	| CTS,DCD			| \<start address\> \<size> \<access speed\>					    |
| PIA8255		| 8255 PIA								| R/W			| LOW_RATE (50 Hz)		|					|				| 					| \<start address\> \<size> \<access speed\>						|
| VIA6522		| 6522 VIA								| R/W			| MICROPROCESSOR_RATE	|					|				|					| \<start address\> \<size> \<access speed\> \<clock speed\>        |
| BeebSerULA	| BBC Micro Serial ULA					| R/W			| MICROPROCESSOR_RATE	|					|				| TxD,RTSI			| \<start address\> \<size> \<access speed\>						|
| ADC7002       | 12-bit Analogue-to-Digital Converter  | R/W			| MICROPROCESSOR_RATE	|					|				|					| \<start address\> \<size> \<access speed\> \<clock speed\>        |
| ZN428         | 8-bit Digital-to-Analogue Converter   | W 			| NONE					|					|				|					| \<start address\> \<size> \<access speed\>						|
| CPU_6502		| 6502 Instruction-stepped				|				| MICROPROCESSOR_RATE	|					|				|					| \<clock speed\>													|
| CPU_6502CC	| 6502 Micro cycle-stepped				|				| MICROPROCESSOR_RATE	|					|				|					| \<clock speed\>													|
| TAPREC		| Tape Recorder							|				| MICROPROCESSOR_RATE	|					|				|					|																	|
| TI4689		| TI4689 Tone Generator					| R/W			| HI_RATE (31.25 kHz)	|					|				| WE,D				|																	|
| SD_CARD		| SD Card with SPI interface			|				| NONE					|					|				| CLK,MOSI			|																	|
| ATOMKB		| Acorn Atom Keyboard					|				| LOW_RATE (50 Hz)		|					|				| ROW				|																	|
| BEEBKB        | BBC Micro Keyboard				    |				| LOW_RATE (50 Hz)		|					|				|					| \<keyboard start-up option byte\>	\<clock speed\>					|
| ATOMCAS		| Acorn Atom Cassette Interface			|				| MICROPROCESSOR_RATE	|					|				|					|																	|
| ATOMSP		| Acorn Atom Sound Device				|				| HI_RATE (31.25 kHz)	|					|				|					|																	|
| 74LS259		| 74LS259 8-bit Addressable Latch		|				| NONE					|					|				|					|																	|