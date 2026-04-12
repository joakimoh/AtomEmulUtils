# The Menu Bar
## The File Menu
The File Menu allows you to load data from a file directly into memory.
For this to work, the data file needs to be accompanied by an .INF file
with information about the load address. E.g., if you want to load data
from a file 'GEST.bin' to memory locations 0xe00 and forwards, then a file
GEST.inf with the content
```
GTEST	0e00
```
needs to be created. The base part of the data file and the .INF files not
to match.

To save memory data to file, the file name needs to include both the start
and end address of the data area to save. To save e.g, the memory content
0xe00 to 0xeff, the file name shall be similar to 'data_e000_eff.bin'. The file
extension or naming of the file doesn't matter.

![The File Menu.](File.jpg)

## The Memory Card Menu
![The Memory Card Menu.](MemoryCard.jpg)

## The Screen Refresh Rate Menu
![The  Screen Refresh Menu.](RefreshRate.jpg)

## The Speed Menu
![The Speed Menu.](Speed.jpg)

## The Tape Recorder Menu
![The Tape Recorder Menu.](TapeRecorder.jpg)

## The Debugger Menu
![The Debugger Menu.](Debugger.jpg)
