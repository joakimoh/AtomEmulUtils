# The Debugger
The debugger is command line only and runs in the terminal window (console) from which you started the
emulator from. It support basic features like breakpoints, tracing and access to both
memory and status of the hardware components. Writing 'help' will give show you the list
of available debugger commands.

## Accessing memory
```
read <hex start address> [<hex end address>]:       read memory content - default is end address = start address
write <hex start address> <hex byte> { <hex byte> }: write bytes to memory
swrite <hex start address> "<string>":              write ASCII string to memory
```

## Accessing the microprocessor

```
up:                                                 get the microprocessor's state
rset (A|X|Y|SP|SR|PC) <hex val>:                    set a register value
reset:                                              reset the microprocessor
```


## Accessing devices
```
devices:                                            lists the devices
state <name of device>:                             get a device's state
```

## Accessing device ports

```
ports <device name>:                                list the ports of a device
pwrite <dev name>:<port name>[<qualifier>]<hex val>:set a device's port value. <qualifier> ::= <bit no> | [<high bit no>;<low bit no>]
pread <dev name>:<port name>:                       get a device's port value
awrite <dev name>:<port name>:                      set a device's analogue input port's value
aread <dev name>:<port name>:                       get a device's analogue port's value


```

## Disassembly
```
dis <hex start address> [<hex end address>]         disassemble memory content - default is 10 instructions

```

## Breakpoints and logging
Only one breakpoint can be set, but it can be linked to the opcode fech address, a memory read address or a memory write address:
```
break x <hex address>:                              continue execution until the program counter reaches the specified address
break r|w|rw <hex address>:                         continue execution until the specified address is accessed in the way specified
break clr:                                          clear any previously set breakpoint
```
It is possible to step a no of isntructions:
```
step [<no of instructions>]:                        execute the specified no of instructions (default is 1) and then stop (instruction tracing only)

```
While stepping, an instruction log will be shown. That log can be extended with both device port status and devices' complete status:
```
mlog (set <adr> | clr):                             add logging of a specific memory address to the instruction log
plog (set <dev>:<prt> {,...<dev:<prt>} | clr):      add (or completely remove) logging of specific device ports to the instruction log
dlog (set <device> {,...<device>} | clr):           add logging of specific devices' states to the instruction log
skip:                                               as 'step 1' but will step over a JSR instruction
```
The microprocessor's running state ('running' or 'halt') is a prefix to the prompt '>'.
If the state is 'running', it is possible to stop the microprocessor exection using the
'halt' command. You can resume execution using the 'cont' command:
```
cont:                                               continue execution (if previusly stopped)
halt:                                               stop execution

```

## Tracing
```
twin (set <sz> | clr):                              enable trace window of a certain size or disable it
dsel (set <device> {,...<device>} | clr):           select the devices to be part of the extensive tracing
notrace:                                            turn off extensive tracing
trace <string with from letters below> [<n>]:   turn on extensive tracing for the devices selected by the dsel command. Stop after <n> instructions if specified.
        'V' verbose output
        'e' errors
        'w' warnings
        'u' microprocessor execution
        'p' device port updates
        'i' interrupts & reset
        'r' only reset
        'k' keyboard
        'g' graphics <=> video display units
        's' serial/parallel I/O peripherals
        'a' audio
        'd' device execution in general
        't' triggering on R/W accesses
        'c' cassette tape I/O
        'x' measuring execution time of the different components
        'S' SPI devices
        'C' ADC devices
        'X' extensive debugging for the selected debug scope
        'A' all the above

```

## Memory map
The emulated system's memory map can be shown:

```
map:                                                print memory map
```

## Misc
Misc. commands are listed below:
```
help												list available commands
exit:                                               exit the debugger
settings:                                           output the current settings (breakpoints, selected devices for logging etc.)
```
