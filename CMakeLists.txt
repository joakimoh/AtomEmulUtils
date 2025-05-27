# CMakeList.txt : CMake project for Assemblerutils
#
cmake_minimum_required (VERSION 3.8)
project ("AssemblerUtils" VERSION 2.2)
include(FetchContent)

#option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Turn of warnings to get allegro to compile without errors
if(CMAKE_COMPILER_IS_GNUCC)
    set(WFLAGS "-Wno-narrowing")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(WFLAGS "-Wno-narrowing")
endif()
if(COMPILER_MSVC)
	set(WFLAGS "${WFLAGS} -Wno-c++11-narrowing")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /GL /OPT:REF")
endif(COMPILER_MSVC)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WFLAGS}")

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_BUILD_TYPE Release)

message(CMAKE_C_COMPILER_ID="${CMAKE_C_COMPILER_ID}")
message(CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS}")

FetchContent_Declare(
  allegro5
  GIT_REPOSITORY https://github.com/liballeg/allegro5.git
  GIT_TAG        5.2.10.1
)
FetchContent_GetProperties(allegro5)
if(NOT allegro5_POPULATED)
  FetchContent_Populate(allegro5)
	if (MSVC)
		set(SHARED ON)
	else()
		set(SHARED OFF)
	endif()
	set(WANT_TESTS OFF)
	set(WANT_EXAMPLES OFF)
	set(WANT_DEMO OFF)
  add_subdirectory(${allegro5_SOURCE_DIR} ${allegro5_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
#set(WANT_STATIC_RUNTIME "Whether or not to link the C and C++ runtimes statically (currently only implemented on Windows)" OFF)

# Locate zlib
find_package(ZLIB REQUIRED)



set (MY_ALLEGRO_SOURCE_DIR ${allegro5_SOURCE_DIR})
set (MY_ALLEGRO_BINARY_DIR ${allegro5_BINARY_DIR})

# Create sub driectory for shared library
add_subdirectory(shared)

# Common
include_directories(
    "${PROJECT_BINARY_DIR}"
	"${CMAKE_SOURCE_DIR}/shared"	
	${allegro5_SOURCE_DIR}/include
	${allegro5_BINARY_DIR}/include
	${ZLIB_INCLUDE_DIRS}
)

# Dis application
add_executable ( 
	Dis  "Dis/Dis.cpp"
	"Dis/ArgParser.h"  "Dis/ArgParser.cpp"
 )
target_link_libraries(Dis PUBLIC shared )

# Emu6502 application
add_executable ( 
	Emu6502  "Emu6502/Emu6502.cpp"
	"Emu6502/ArgParser.h"  "Emu6502/ArgParser.cpp"
 )

target_include_directories(Emu6502 PUBLIC ${allegro5_SOURCE_DIR}/include)
target_include_directories(Emu6502 PUBLIC ${allegro5_BINARY_DIR}/include)
target_link_libraries(Emu6502 PUBLIC shared allegro allegro_main allegro_font allegro_primitives allegro_image allegro_dialog allegro_audio PRIVATE ZLIB::ZLIB)

# These include files are typically copied into the correct places via allegro's install
# target, but we do it manually.
file(COPY ${allegro5_SOURCE_DIR}/addons/font/allegro5/allegro_font.h
	DESTINATION ${allegro5_SOURCE_DIR}/include/allegro5
)
file(COPY ${allegro5_SOURCE_DIR}/addons/primitives/allegro5/allegro_primitives.h
	DESTINATION ${allegro5_SOURCE_DIR}/include/allegro5
)
file(COPY ${allegro5_SOURCE_DIR}/addons/image/allegro5/allegro_image.h
	DESTINATION ${allegro5_SOURCE_DIR}/include/allegro5
)
file(COPY ${allegro5_SOURCE_DIR}/addons/native_dialog/allegro5/allegro_native_dialog.h
	DESTINATION ${allegro5_SOURCE_DIR}/include/allegro5
)
file(COPY ${allegro5_SOURCE_DIR}/addons/audio/allegro5/allegro_audio.h
	DESTINATION ${allegro5_SOURCE_DIR}/include/allegro5
)

# Installation of Disassembler
install(TARGETS Dis DESTINATION bin)
install(
	FILES "Dis/Dis.h"
    DESTINATION include/Dis
)

# Installation of Emulator
install(TARGETS Emu6502 DESTINATION bin)
install(
	FILES "Emu6502/Emu6502.h"
    DESTINATION include/Dis
)



# Installer (will be created if cpack --config CPackConfig.cmake + cpack --config CPackSourceConfig.cmake are run from command line)
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}")
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_DIRECTORY "distribution")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_NAME "AssemblerUtils")
set(CPACK_PACKAGE_VERSION_MAJOR "${AtomTapeUtils_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${AtomTapeUtils_VERSION_MINOR}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Utility to disassemble 6502 machine code")
set(CPACK_PACKAGE_DESCRIPTION
"This is a utility to disassemble 6502 machine code")
IF (WIN32)
  set(CPACK_GENERATOR WIX)
ELSEIF(UNIX)
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
  #set(CPACK_DEBIAN_PACKAGE_NAME "AssemblerUtils-dev")
  #set(CPACK_DEBIAN_PACKAGE_VERSION 1.0.0)
  #set(CPACK_DEBIAN_PACKAGE_RELEASE 1)
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Joakim Ohlsson")
ENDIF()
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)

