# The configuration (map) file
The map file is the one that defines your computer system. It has different sections (the order of these sections
doesn't matter and you can mix directives as you like in the map file but keeping similar directives in the same
section will easy readability and is therefore recommended).

## The memory mapping section
Here you specify all the devices (including the ones that are not memory-mapped actually).
Each device is added with the directive
```
ADD <device type> <your alias for the device>  <device parameters>
```
Where _device type_ is the selected component (e.g. 'DRAM') and _your alias for the device_
is a unique name you give that component instance (e.g. 'IC11'). The same type of component
can be be used in several places in the same computer system and each instance then will have a unique
name (e.g. 'IC11' and 'IC12' for two DRAM devices). The name needs also to be unique among all
devices. This is because the name will later be used when defining device ports that shall
be interconnected.

The _device parameters_ will be sligtly different for different types of devices.
A list of exactly  which parameters each device type need to have can be found under
[Device Summary](#ldevice-summary).
Below follows a general description of the parameters used for most devices.

Memory-mapped devices have at least the parameters:
```
<start address> <size> <access speed>
```
_start address_ is the lowest address at which the device can be accessed by
the microcontroller (or by a video data unit) and _size_ is the size in
bytes of the space the device occupies in memory and _access speed_ is the
max speed (in mHz) at which the device can be accessed (this information is
used to decide what type of clock stretching will be required - if enabled).

Devices advance time either based on the emulation engines base time ticks or the device's
own clock (for some clocked devices). In the latter case, a clock speed
parameter needs also to be specified.

The example below defines a device _SYS_VIA_ of type _VIA6522_, located at address
0xfe40 to 0xfe5f, with an access speed of 1 MHz and an input clock of 1 MHz.
```
ADD	VIA6522			SYS_VIA		fe40	0020	1			1.0
```
Video devices can have a parameter that species the start address of the
video memory.
```
<video mem adr>
```

The example below defines a device _VDU__ of type _VDU6847_, located at address
0x800 to 0x8ff, with an access speed of 1 MHz and accessing video memory from
address 0x8000 and updwards.
```
ADD	VDU6847		VDU			0800	0100		1			8000
```

Memory-mapped devices (normally a memory device like a RAM or a ROM) is also allowed
to have small gaps in the memory space they occupy. Such a gap is specified by
the _GAP_ directive.

The example below defines a memoeroy device _OS__ of type _ROM_, located at address
0c000 to 0xffff with a gap at 0xfc00 to 0xeff, with an access speed of 2 MHz and with
concent from the file 'OS12.rom'.
```
ADD	ROM				OS			c000	4000	2			OS12.rom	// OS (IC51)		 				c000 - ffff
GAP					OS			fc00	0300							// Gap in OS ROM space between		fc00 - ff00
```

## Device Summary
The table below lists all devices and specifies whether they are memory-mapped, how they are scheduled, whether immediate port processing is required and the parameters they need to have.
Scheduling is a combination of rate-based scheduling (column _Rate Scheduling_) and triggering by call from other devices (Columns _At Call_ and _Callees_).


| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| DRAM			| DRAM									| R/W			|						|					|				|					| \<start address\> \<size> \<access speed\>						|
| SRAM			| Static RAM							| R/W			|						|					|				|					| \<start address\> \<size> \<access speed\>						|
| ROM			| ROM									| W				|						|					|				|					| \<start address\> \<size> \<access speed\> \<ROM file\>			|
| CRTC6845      | H6845 Character Raster Generator      | R/W			|						| Advance one Char	| BeebVideoULA	| 					| \<start address\> \<size> \<access speed\>						|
| SAA5050		| SAA5050 Teletext Character Generator	| R/W			|						| Advance one Char	| BeebVideoULA	| DEW,GLR,LOSE		|																	|
| BeebVideoULA	| BBC Micro Video ULA					| R/W			| MICROPROCESSOR_RATE	|					|				| RA,VS,HS,C		| \<start address\> \<size> \<access speed\>						|
| VDU6847		| MC6847 Video Display Generator		| R/W			| MICROPROCESSOR_RATE	|					|				| A/S,GM			| \<start address\> \<size> \<access speed\> \<video mem adr\>		|																		|
| BeebRomSel	| BBC Micro Paged Memory Selection		| W				|						|					|				|					| \<start address\> \<size> \<access speed\>						|
| ACIA6850		| M6850 ACIA							| R/W			| MICROPROCESSOR_RATE	| Poll Rx/TX CLK	| BeebSerULA	| CTS,DCD			| \<start address\> \<size> \<access speed\>					    |
| PIA8255		| 8255 PIA								| R/W			| LOW_RATE (50 Hz)		|					|				| 					| \<start address\> \<size> \<access speed\>						|
| VIA6522		| 6522 VIA								| R/W			| MICROPROCESSOR_RATE	|					|				|					| \<start address\> \<size> \<access speed\> \<clock speed\>        |
| BeebSerULA	| BBC Micro Serial ULA					| R/W			| MICROPROCESSOR_RATE	|					|				| TxD,RTSI			| \<start address\> \<size> \<access speed\>						|
| ADC7002       | 12-bit Analogue-to-Digital Converter  | R/W			| MICROPROCESSOR_RATE	|					|				|					| \<start address\> \<size> \<access speed\> \<clock speed\>        |
| ZN428         | 8-bit Digital-to-Analogue Converter   | W 			|						|					|				|					| \<start address\> \<size> \<access speed\>						|
| CPU_6502		| 6502 Instruction-stepped				|				| MICROPROCESSOR_RATE	|					|				|					| \<clock speed\>													|
| CPU_6502CC	| 6502 Micro cycle-stepped				|				| MICROPROCESSOR_RATE	|					|				|					| \<clock speed\>													|
| TAPREC		| Tape Recorder							|				| MICROPROCESSOR_RATE	|					|				|					|																	|
| TI4689		| TI4689 Tone Generator					| R/W			| HI_RATE (31.25 kHz)	|					|				| WE,D				|																	|
| SD_CARD		| SD Card with SPI interface			|				|						|					|				| CLK,MOSI			|																	|
| ATOMKB		| Acorn Atom Keyboard					|				| LOW_RATE (50 Hz)		|					|				| ROW				|																	|
| BEEBKB        | BBC Micro Keyboard				    |				| LOW_RATE (50 Hz)		|					|				|					| \<keyboard start-up option byte\>	\<clock speed\>					|
| ATOMCAS		| Acorn Atom Cassette Interface			|				| MICROPROCESSOR_RATE	|					|				|					|																	|
| ATOMSP		| Acorn Atom Sound Device				|				| HI_RATE (31.25 kHz)	|					|				|					|																	|
| 74LS259		| 74LS259 8-bit Addressable Latch		|				|						|					|				|					|																	|

Triggering by call means that another device (the callee) exchanges data with (and sometimes also advances the time of) the called device by explicitly calling a method provided by that device.
The result is that data is exchanged between the two devices and possibly, the time is also advanced for the called device. An example of this it that the BeebVideoULA device calls both the SAA5050
and CRTC6845 devices to advance their time corresponding to one screen character. At the same time the BeebVideoULA device will get character data from those devices to use when generating 
video output. Only the devices listed in the _Callees_ column can be connected to the called device. Another example is that the BeebSerULA communicates changes in the received and transmit
clock speeds to the ACIA6850 device (here  time is not advanced of the called device - only data is exchanged).
The calling is  specified by the TRIGGER directive.

Example:
```
TRIGGER	CRTC		CALL	VDU
```

## Complete list of directives
The map file can have many lines. Each line can have a directive that specifies a device, how it is connected or scheduled and more.
A blank line (or a line just consisting of white space) will be ignored. Any part of a line that starts with '\\\\' will be the
start of a comment and will also be ignored.

### Constants
The emulator uses as aa default a tick rate equal to the clock rate of the microprocessor. But
it is possible to select another one (needs to be at least as fast as the clock rate of the microprocessor) using the _TICK_RATE_ directive:
```
TICK_RATE			 <rate in MHz>
```

The video standard defaults to that of the selected video device but another one can still be specified using
the VIDEO directive:
```
VIDEO				('PAL' | 'NTSC')
```

The input clock to the microprocessor will be 'stretched' based on the access speed of the memoery device being
accessed by the microprocessor if clock stretching is enabled (default it is disabled). To enable it, use the
_CLOCK_STRETCHING_ directive:
```
CLOCK_STRETCHING	('ON' | 'OFF')
```

The base rate of the emulator defaults to 50 Hz unless another one is selected using the EMU_LOW_RATE directive:
```
EMU_LOW_RATE		<rate in Hz>
```

The high device schedule rate defaults to 31.25 kHz unless another one is selected using the EMU_HIGH_RATE directive:
```
EMU_HIGH_RATE		<rate in Hz>
```

### Device Declarations
To declare a device, the _ADD_ directive shall be used:
```
ADD <device type> <your alias for the device>  <device parameters>
```
For details about the parameters for a specific type of device, see [Device Specifications](#device-specifications).

If the device is memory-mapped one and it doesn't occupy a continuous memory space, gaps in the memory space can
be specified using the _GAP_ directive:
```
GAP <device> <start address> <size>
```

### Connecting devices
To connect one digital output port of one device (the source device) to another device's (the destination device)
input port, the directive _CONNECT_ shall be used:
```
CONNECT	<src device>':'<port>[';'[<high bit>[';'<low bit>]] <dst device>':'<port>[';'[<high bit>[';'<low bit>]]
```
It is possible to select a single bit or a subset of the bits of both the soruce and destination ports.

To connect one analogue output port of one device (the source device) to another device's (the destination device)
analogue input port, the directive _ACONNECT_ shall be used:
```
ACONNECT	<src device>':'<port>	<dst device>':'<port>
```

### Device scheduling
The rate at which a device is scheduled (if it should be rate-scheduled) needs to be specified using the _SCHED_ directive:
```
SCHED	<device>	('MICROPROCESSOR_RATE' | 'LOW_RATE' | HIGH_RATE')
```
_MICROPROCESSOR_RATE_ is the step rate of the microprocessor.

A Device can also be scheduled using the _TRIGGER_ directive.
To trigger the device at memory read accesses:

```
TRIGGER	<device>	MEM		READ	<hex address>
```
To trigger the device at memory write accesses:
```
TRIGGER	<device>	MEM		WRITE	<hex address>
```
The last way to trigger a device is by call by another device. This is
specified using _CALL_:
```
TRIGGER <device>	CALL	<calling device>
```

### Port initialisation
A digital device port can be initialised to another value then the device's default value at initialisation by the use
of the INIT directive:
```
INIT	<device>':'<port>	<hex value>
```

Also an analogue device port can be initialised to another value then the device's default value at initialisation.
This is made using the AINIT directive:
```
AINIT 	<device>':'<port>	<hex value>
```

## Device Specifications
Below follows a description of each device as required to understand how to add it to your computer system.
For a good understanding a specific device's detailed operation you should also read the device's datasheet (links provided for each device)
as such a detailed description will not be provided here.

### DRAM
The DRAM emulates a Dynamic RAM. The content will be cleared at initialisation.
No reference to any DRAM datasheet will be provided here as it is assumed that the user can understand the operation of such a basic device
without it.

#### Parameters
| Parameter		| Description												|
| ------------- | --------------------------------------------------------- |
| start address	| Start of the memory space occupied by the device			|
| size			| Size of the memory space occupied by the device			|
| access speed	| Speed in MHz for microprocessor accesses to the device	|


#### Ports
| Port			| Type			| Direction		| Size		| Polarity		| Default Value		| Description							| Connect to the input						|
| ------------- | ------------- | ------------- | --------- | ------------- | ----------------- | ------------------------------------- | ----------------------------------------- |
| CS			| Digital		| INPUT			| 1 bit		| Active LOW	| 0					| Chip Select							| CONNECTION								|

#### Scheduling
The device doesn't have any notion of time and doesn't have to be scheduled.

### SRAM
The SRAM emulates a Static RAM. The content will be initialised with random values at initialisation.
No reference to any SRAM datasheet will be provided here as it is assumed that the user can understand the operation of such a basic device
without it.

#### Parameters
| Parameter		| Description												|
| ------------- | --------------------------------------------------------- |
| start address	| Start of the memory space occupied by the device			|
| size			| Size of the memory space occupied by the device			|
| access speed	| Speed in MHz for microprocessor accesses to the device	|


#### Ports
| Port			| Type			| Direction		| Size		| Polarity		| Default Value		| Description							| Connect to the input						|
| ------------- | ------------- | ------------- | --------- | ------------- | ----------------- | ------------------------------------- | ----------------------------------------- |
| CS			| Digital		| INPUT			| 1 bit		| Active LOW	| 0					| Chip Select							| CONNECTION								|

#### Scheduling
The device doesn't have any notion of time and doesn't have to be scheduled.

### ROM
The ROM emulates a ROM. The content will loaded from a file at initialisation.
No reference to any ROM datasheet will be provided here as it is assumed that the user can understand the operation of such a basic device
without it.

#### Parameters
| Parameter		| Description												|
| ------------- | --------------------------------------------------------- |
| start address	| Start of the memory space occupied by the device			|
| size			| Size of the memory space occupied by the device			|
| access speed	| Speed in MHz for microprocessor accesses to the device	|
| ROM file		| Name of file that contains the ROM content				|


#### Ports
| Port			| Type			| Direction		| Size		| Polarity		| Default Value		| Description							| Connect to the input						|
| ------------- | ------------- | ------------- | --------- | ------------- | ----------------- | ------------------------------------- | ----------------------------------------- |
| CS			| Digital		| INPUT			| 1 bit		| Active LOW	| 0					| Chip Select							| CONNECTION								|

#### Scheduling
The device doesn't have any notion of time and doesn't have to be scheduled.

### CRTC6845

The CRTC6845 emulates a H6845 Character Raster Generator.
A datasheet can be found here: https://www.cpcwiki.eu/imgs/c/c0/Hd6845.hitachi.pdf.

#### Parameters
| Parameter		| Description												|
| ------------- | --------------------------------------------------------- |
| start address	| Start of the memory space occupied by the device			|
| size			| Size of the memory space occupied by the device			|
| access speed	| Speed in MHz for microprocessor accesses to the device	|


#### Ports
| Port			| Type			| Direction		| Size		| Polarity		| Default Value		| Description							| Connect to the input						|
| ------------- | ------------- | ------------- | --------- | ------------- | ----------------- | ------------------------------------- | ----------------------------------------- |
| CLK			| Digital		| INPUT			| 3 bits	| N/A			| 1 (1 MHz)			| Sets the device clock					| CONNECTION								|
| RESET			| Digital		| INPUT			| 1 bit		| Active LOW	| 1					| Resets the device						| CONNECTION								|
| DISPTMG		| Digital		| OUTPUT		| 1 bit		| Active HIGH	| 0					| Active display						|											|	
| RA			| Digital		| OUTPUT		| 5 bits	| N/A			| 0					| Raster address						|											|
| CUDISP		| Digital		| OUTPUT		| 1 bit		| Active HIGH	| 0					| Cursor active							|											|
| HS			| Digital		| OUTPUT		| 1 bit		| Active HIGH	| 0					| Horizontal sync						|											|
| VS			| Digital		| OUTPUT		| 1 bit		| Active HIGH	| 0					| Vertical sync							|											|

#### Scheduling
The device is scheduled by calling it each time a new character is needed. The following devices can call the device: BeebVideoULA.

### Call API (used by the callee device)
```
bool getMemFetchAdr(uint16_t& adr);
```

### SAA5050
The SAA5050 emulates a Mullard SAA5050 Teletext Character generator.
A datasheet can be found here: https://www.cpcwiki.eu/imgs/9/9e/Mullard_SAA5050_datasheet.pdf.

#### Parameters
| Parameter		| Description												|
| ------------- | --------------------------------------------------------- |
| access speed	| Speed in MHz for microprocessor accesses to the device	|


#### Ports

| Port			| Type			| Direction		| Size		| Polarity		| Default Value		| Description							| Connect to the input						|
| ------------- | ------------- | ------------- | --------- | ------------- | ----------------- | ------------------------------------- | ----------------------------------------- |
| GLR			| Digital		| INPUT			| 1 bit		| Active HIGH	| 1					| Start of line							| CONNECTION:P								|
| LOSE			| Digital		| INPUT			| 1 bit		| Active HIGH	| 0					| Start of visible line					| CONNECTION:P								|
| DEW			| Digital		| INPUT			| 1 bit		| Active HIGH	| 1					| Start of field						| CONNECTION:P								|	
| CRS			| Digital		| INPUT			| 1 bit		| Active HIGH	| 0					| Field index (odd=0 or even=1)			| CONNECTION								|

#### Scheduling
The device is sThe device is scheduled by calling it each time a new character is needed. The following devices can call the device: BeebVideoULA.

### Call API (used by the callee device)
```
bool getScreenData(uint8_t pageData, ScreenDataType* &screenData, TTColour &bgColour, TTColour &fgColour);
```

### BeebVideoULA
The BeebVideoULA emulates a BBC Micro Video ULA.
There is no datasheet for this circuit available but a general description of it can be found here: https://beebwiki.mdfs.net/Video_ULA

#### Parameters
| Parameter		| Description												|
| ------------- | --------------------------------------------------------- |
| start address	| Start of the memory space occupied by the device			|
| size			| Size of the memory space occupied by the device			|
| access speed	| Speed in MHz for microprocessor accesses to the device	|


#### Ports
| Port			| Type			| Direction		| Size		| Polarity		| Default Value		| Description							| Connect to the input						|
| ------------- | ------------- | ------------- | --------- | ------------- | ----------------- | ------------------------------------- | ----------------------------------------- |
| C				| Digital		| INPUT			| 2 bits	| N/A			| 0					| Scroll control[^1]					| CONNECTION:P								|
| DISEN			| Digital		| INPUT			| 1 bit		| Active HIGH	| 1					| DISPTMG from the CRTC 6845			| CONNECTION								|
| CURSOR		| Digital		| INPUT			| 1 bit		| Active HIGH	| 0					| CUDISP from the CRTC 6845				| CONNECTION								|
| INV			| Digital		| INPUT			| 1 bit		| Active LOW	| 1					| Invert Video (non-teletext mnodes)	| CONNECTION								|
| RA			| Digital		| INPUT			| 4 bits	| N/A			| 0					| Raster address from the CRTC 6845		| CONNECTION:P								|
| HS			| Digital		| INPUT			| 1 bit		| Active HIGH	| 1					| Horizontal sync						| CONNECTION:P								|
| VS			| Digital		| INPUT			| 1 bit		| Active HIGH	| 1					| Vertical sync							| CONNECTION:P								|
| CRT_CLK		| Digital		| OUTPUT		| 3 bits	| N/A			| 1 (1 MHz)			| Sets the clock of the H6846 CRTC		|											|

[^1]:See https://beebwiki.mdfs.net/Address_translation, Section _Calculation of the adjusted address_. The C1 C0 Columns correspond to the C port above and will be used to
adjust the video memory address to care for scrolling.

#### Scheduling
The device is scheduled at rate MICROPROCESSOR_RATE.

### VDU6847
The VDU6847 emulates an M6847 Video Display Generator.
A datasheet can be found here: https://web.mit.edu/6.115/www/document/MC6847.pdf.

#### Parameters
| Parameter		| Description												|
| ------------- | --------------------------------------------------------- |
| start address	| Start of the memory space occupied by the device			|
| size			| Size of the memory space occupied by the device			|
| access speed	| Speed in MHz for microprocessor accesses to the device	|
| video adr		| Start address  of the video memory						|


#### Ports
| Port			| Type			| Direction		| Size		| Polarity		| Default Value		| Description							| Connect to the input						|
| ------------- | ------------- | ------------- | --------- | ------------- | ----------------- | ------------------------------------- | ----------------------------------------- |
| A/S			| Digital		| INPUT			| 1 bit		| Active HIGH	| 0					| alphanumeric (0) or semigraphics (1)	| CONNECTION:P								|
| A/G			| Digital		| INPUT			| 1 bit		| Active HIGH	| 0					| alphanum./semigr. (0) or graphics (1)	| CONNECTION								|
| GM			| Digital		| INPUT			| 3 bits	| N/A			| 0					| graphic mode (0-7)					| CONNECTION:P								|
| CSS			| Digital		| INPUT			| 1 bit		| Active HIGH	| 0					| colour palette						| CONNECTION								|
| INT/EXT		| Digital		| INPUT			| 1 bit		| Active LOW	| 0					| external char ROM (1) or internal (0)	| CONNECTION								|
| INV			| Digital		| INPUT			| 4 bits	| N/A			| 0					| inverted character					| CONNECTION								|
| FS			| Digital		| OUTPUT		| 1 bit		| Active HIGH	| 1					| Field Sync							|											|
| DIn			| Digital		| OUTPUT		| 8 bits	| N/A			| 0					| Mirror of DIn read by the device[^2]	|											|

[^2]:This is a trick to make it possible to connect a data in pin to another pin on the device. For an Acorn Atom e.g., bit 7 is conencted to the INV input pin.

#### Scheduling
The device is scheduled at rate MICROPROCESSOR_RATE.
																		
### BeebRomSel
The BeebRomSel emulates the paged ROM selection circuitry of a BBC Micro. It is extended sligtly to support selection of up to 16 memory devices.
See IC20 (74LS139) and IC76 (74LS163) on the BBC Micro Model B circuit diagram at  https://stardot.org.uk/forums/download/file.php?id=34864 and https://beebwiki.mdfs.net/Paged_ROM for details.

#### Parameters
| Parameter		| Description												|
| ------------- | --------------------------------------------------------- |
| start address	| Start of the memory space occupied by the device			|
| size			| Size of the memory space occupied by the device			|
| access speed	| Speed in MHz for microprocessor accesses to the device	|

#### Ports
| Port			| Type			| Direction		| Size		| Polarity		| Default Value		| Description							| Connect to the input						|
| ------------- | ------------- | ------------- | --------- | ------------- | ----------------- | ------------------------------------- | ----------------------------------------- |
| NW			| Digital		| OUTPUT		| 1 bit		| Active LOW	| 1					| Slot 12 Selection						|											|
| NE			| Digital		| OUTPUT		| 1 bit		| Active LOW	| 1					| Slot 13 Selection						|											|
| SW			| Digital		| OUTPUT		| 3 bits	| Active LOW	| 1					| Slot 14 Selection						|											|
| SE			| Digital		| OUTPUT		| 1 bit		| Active LOW	| 1					| Slot 15 Selection						|											|
| B0			| Digital		| OUTPUT		| 4 bits	| N/A			| f					| Selection of slots 0-3				|											|
| B1			| Digital		| OUTPUT		| 4 bits	| N/A			| f					| Selection of slots 4-7				|											|
| B2			| Digital		| OUTPUT		| 4 bits	| N/A			| f					| Selection of slots 8-11				|											|


#### Scheduling
The device is not explcitly scheduled. The outputs are updated by writing to the device.

### ACIA6850
The ACIA6850 emulates an M6850 ACIA.
A datasheet can be found here: https://www.cpcwiki.eu/imgs/3/3f/MC6850.pdf.

#### Parameters
| Parameter		| Description												|
| ------------- | --------------------------------------------------------- |
| start address	| Start of the memory space occupied by the device			|
| size			| Size of the memory space occupied by the device			|
| access speed	| Speed in MHz for microprocessor accesses to the device	|

#### Ports
| Port			| Type			| Direction		| Size		| Polarity		| Default Value		| Description							| Connect to the input						|
| ------------- | ------------- | ------------- | --------- | ------------- | ----------------- | ------------------------------------- | ----------------------------------------- |
| RxD			| Digital		| INPUT			| 1 bit		| Active LOW	| 1					| Serial receive data					| CONNECTION								|
| CTS			| Digital		| INPUT			| 1 bit		| Active LOW	| 1					| Clear To Send							| CONNECTION:P								|
| DCD			| Digital		| INPUT			| 1 bit		| Active LOW	| 1					| Data Carrier Detect					| CONNECTION:P								|
| TxD			| Digital		| OUTPUT		| 1 bit		| Active LOW	| 1					| Serial send data						| CONNECTION								|
| RTS			| Digital		| OUTPUT		| 1 bit		| Active LOW	| 1					| Return To Send						| CONNECTION								|
| IRQ			| Digital		| OUTPUT		| 1 bit		| Active LOW	| 0					| Interrupt Request						| CONNECTION								|

#### Scheduling
The device is not scheduled on MICROPROCESSOR_RATE rate.
The Rx and Tx clocks are not available as ports (for performance reasons) and are instead set by a method call to the device. The devices that can call the ACIA are: BeebSerULA.

### Call API (used by the callee device)
```
void setRxClkRate(long clkRate);
void setRxClkRate(long clkRate);
```





### PIA8255
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| PIA8255		| 8255 PIA								| R/W			| LOW_RATE (50 Hz)		|					|				| 					| \<start address\> \<size> \<access speed\>						|

### VIA6522
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| VIA6522		| 6522 VIA								| R/W			| MICROPROCESSOR_RATE	|					|				|					| \<start address\> \<size> \<access speed\> \<clock speed\>        |

### BeebSerULA
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| BeebSerULA	| BBC Micro Serial ULA					| R/W			| MICROPROCESSOR_RATE	|					|				| TxD,RTSI			| \<start address\> \<size> \<access speed\>						|

### ADC7002
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| ADC7002       | 12-bit Analogue-to-Digital Converter  | R/W			| MICROPROCESSOR_RATE	|					|				|					| \<start address\> \<size> \<access speed\> \<clock speed\>        |

### ZN428
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| ZN428         | 8-bit Digital-to-Analogue Converter   | W 			| NONE					|					|				|					| \<start address\> \<size> \<access speed\>						|

### CPU_6502
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| CPU_6502		| 6502 Instruction-stepped				|				| MICROPROCESSOR_RATE	|					|				|					| \<clock speed\>													|

### CPU_6502CC
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| CPU_6502CC	| 6502 Micro cycle-stepped				|				| MICROPROCESSOR_RATE	|					|				|					| \<clock speed\>													|

### TAPREC
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| TAPREC		| Tape Recorder							|				| MICROPROCESSOR_RATE	|					|				|					|																	|

### TI4689
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| TI4689		| TI4689 Tone Generator					| R/W			| HI_RATE (31.25 kHz)	|					|				| WE,D				|																	|

### SD_CARD
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| SD_CARD		| SD Card with SPI interface			|				| NONE					|					|				| CLK,MOSI			|																	|


### ATOMKB
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| ATOMKB		| Acorn Atom Keyboard					|				| LOW_RATE (50 Hz)		|					|				| ROW				|																	|


### BEEBKB
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| BEEBKB        | BBC Micro Keyboard				    |				| LOW_RATE (50 Hz)		|					|				|					| \<keyboard start-up option byte\>	\<clock speed\>					|


### ATOMCAS
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| ATOMCAS		| Acorn Atom Cassette Interface			|				| MICROPROCESSOR_RATE	|					|				|					|																	|


### ATOMSP
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| ATOMSP		| Acorn Atom Sound Device				|				| HI_RATE (31.25 kHz)	|					|				|					|																	|


### 74LS259
| Device        | Description                           | Memory Access	| Rate Scheduling       | At Call           | Callees		| Port Processing	| Parameters														|
| ------------- | ------------------------------------- | ------------- | --------------------- | ----------------- | ------------- | ----------------- | ----------------------------------------------------------------- |
| 74LS259		| 74LS259 8-bit Addressable Latch		|				| NONE					|					|				|					|																	|





