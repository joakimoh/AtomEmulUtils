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

For memory-mapped devices the parameters are:
```
<start address> <size> <access speed> [<clock speed> ]
```
_start address_ is the lowest address at which the device can be accessed by
the microcontroller (or by a video data unit) and _size_ is the siaze in
bytes of the space the device occupies in memory and _access speed_ is the
max speed (in mHz) at which the device can be accessed (this information is
used to decide what type of clock stretching will be required - if enabled).
Optionally (for devices that are clocked) a clock speed can be specified
Example:
```
ADD	VIA6522			SYS_VIA		fe40	0020	1			1.0
```

## List of devices and their parameters
| Device        | Description                           | Parameters                                                        |
| ------------- | ------------------------------------- | ----------------------------------------------------------------- |
| ADC7002       | 12-bit Analogue-to-Digital Converter  | \<start address\> \<size> \<access speed\> \<clock speed\>        |
| ZN428         | 8-bit Digital-to-Analogue Converter   | \<start address\> \<size> \<access speed\> \<clock speed\>        |
| CRT6845       | Character Raster Generator            | \<start address\> \<size> \<access speed\>                        |