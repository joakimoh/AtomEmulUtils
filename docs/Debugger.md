# The Debugger
The debugger is command line only and runs in the terminal window (console) from which you started the
emulator from. It support basic features like breakpoints, tracing and access to both
memory and status of the hardware components. Writing 'help' will give show you the list
of available debugger commands.

## Accessing memory
```
read <hex start address> [<hex end address>]:       read memory content - default is end address = start address
write <hex start address <hex byte> { <hex byte> }: write bytes to memory
swrite <hex start address> "<string>":              write ASCII string to memory

```

## Accessing the microprocessor

```
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
ports <device name>:                                     list the ports of a device
pwrite <dev name>:<port name>[<qualifier>]<hex val>:set a device's input port value. <qualifier> ::= <bit no> | [<high bit no>;<low bit no>
pread <dev name>:<port name>:                       get a device's port value
awrite <dev name>:<port name>:                      set a device's analogue input port's value
aread <dev name>:<port name>:                       get a device's analogue port's value


```

## Disassembly
```
dis <hex start address> [<hex end address>]         disassemble memory content - default is 10 instructions

```

## Breakpoints and logging
```
mlog (set <adr> | clr):                             add logging of a specific memory address to instruction log
up:                                                 get the microprocessor's state
step [<no of instructions>]:                        execute the specified no of instructions (default is 1) and then stop (instruction tracing only)
skip:                                               as 'step 1' but will step over a JSR instruction
cont:                                               continue execution (if previusly stopped)
break x <hex address>:                              continue execution until the program counter reaches the specified address
break r|w|rw <hex address>:                         continue execution until the specified address is accessed in the way specified
break clr:                                          clear any previously set breakpoint
halt:                                               stop execution

```

## Tracing
```
twin (set <sz> | clr):                              enable trace window of a certain size or disable it
dsel (set <device> {,...<device>} | clr):           select the devices to be part of the extensive tracing
plog (set <port> {,...<port>} | clr):               add logging of specific device ports to the trace
dlog (set <device> {,...<device>} | clr):           add logging of specific devices' states to the trace

notrace:                                            turn off extensive tracing
trace <string with from letters below> [<n>]:   turn on extensive tracing for the devices selected by the dsel command. Stop after <n> instructions if specified.

```

## Memory map

```
map:                                                print memory map
```

## Misc
```
help												list available commands
exit:                                               exit the debugger
settings:                                           output the current settings (breakpoints, selected devices for logging etc.)
```
