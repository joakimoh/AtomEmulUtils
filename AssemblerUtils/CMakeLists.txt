# CMakeList.txt : CMake project for AssemblerUtils, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (AssemblerUtils "AssemblerUtils.cpp" "AssemblerUtils.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AssemblerUtils PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
