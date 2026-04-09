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

Now you can configure, build and install:
```
> cd AtomEmulUtils
> cmake -DCMAKE_BUILD_TYPE=Release -S . -B ../AtomEmulUtilsBuild
> cmake --build ../AtomEmulUtilsBuild
> cd ../AtomEmulUtilsBuild
> make install
```
You might also need to export the path to the library and executaables built (replace '...' with whatever
path you have to reach the lib and bin directories):
```
> export LD_LIBRARY_PATH=...AtomEmulUtilsBuild/lib
> PATH=$PATH:.../AtomEmulUtilsBuild/bin
```
This you should add to your ~/.bashrc file.

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

# Install instructions for Mac
I'm sorry but I don't own a Mac and cannot guide you there. The project should build on any machine in theory but
you cannot really tell until you try it. If you do get it to build on a Mac, then please tell me about it and I will update the instruction
to tell about that as well!

# General troubleshooting
If you get errors related to building the allegro5 libraries and there is aline
```
cmake_minimum_required(VERSION 3.12...4.0)
```
in the file AtomEmulUtils/.../_deps/allegro5-src/CmakeList.txt (replace '...' with whatever path you have downto '_deps').
Then change that line to:
```
cmake_minimum_required(VERSION 3.8)
```
