# CMakeList.txt : CMake project for Assemblerutils
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

project ("AssemblerUtils" VERSION 2.2)

# Create sub driectory for shared library
add_subdirectory(shared)

#
include_directories(
    "${PROJECT_BINARY_DIR}"
	"${CMAKE_SOURCE_DIR}/shared"	
	${ZLIB_INCLUDE_DIRS}
)

add_executable ( 
	Dis  "Dis/Dis.cpp"
	"Dis/ArgParser.h"  "Dis/ArgParser.cpp"
 )
target_link_libraries(Dis PUBLIC shared )

add_executable ( 
	Sim  "Sim/Sim.cpp"
	"Sim/ArgParser.h"  "Sim/ArgParser.cpp"
 )
target_link_libraries(Sim PUBLIC shared )

# Installation of Disassembler
install(TARGETS Dis DESTINATION bin)
install(
	FILES "Dis/Dis.h"
    DESTINATION include/Dis
)

# Installation of Emulator
install(TARGETS Sim DESTINATION bin)
install(
	FILES "Sim/Sim.h"
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
"This is a utility to disasemble 6502 machine code")
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

