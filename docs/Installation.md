# Install instructions for Ubuntu
Start by cloning the repository to your computer.
```
> git clone https://https://github.com/joakimoh/AtomEmulUtils.git
```
Install zlib and cmake if not already installed:
```
> sudo apt install zlib1g-dev
> sudo apt install cmake
```

You might also need to install additional libraries (only do it if you get error messages during building):

```
> sudo apt-get install libxi-dev

> sudo apt-get install -y libpng-dev

> sudo apt-get install libjpeg-dev

> sudo apt install libwebp-dev

> sudo apt install libpulse-dev

> sudo apt-get install libopenal-dev

> sudo apt install libgtk-3-dev

> sudo apt install libflac-dev

> sudo apt install libaldmb1-dev

> sudo apt-get install libvorbis-dev

> sudo apt-get install libphysfs-dev

> sudo apt-get install libtheora-dev
```
Now you can build:
```
> cd AtomEmulUtils
> cmake -S . -B build
> cmake --build build
> cmake --build build -t AtomEmulUtils
```

# Install instructions for Windows
Start by installing zlib and Visual Studio and then clone the repository to your computer using Visual Studio.
Then simple select Build All.

## Troubleshooting
cmake might fail to locate where you have installed zlib. If this is the case, then you can try to uncomment the line
```
#set(ZLIB_ROOT "C:/Program Files/zlib")
```
in the file CMakeLists.txt at the project root and update it to point to where you installed zlib (e.g., C:/Program Files/zlib).
Also update the Windows path variable to point to where the DLL is located (e.g., C:/Program Files/zlib/bin).

# General troubleshooting
If you get errors related to buidling the allegro5 libraries you should change the line
```
cmake_minimum_required(VERSION 3.12...4.0)
```
of the file ...\_deps\allegro5-src\CmakeList.txt to
```
cmake_minimum_required(VERSION 3.8)
```
