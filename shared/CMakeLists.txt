add_library(
	shared
	"Codec6502.cpp" "P6502.cpp"
	"Codec6502.h" "P6502.h" "VIA6522.h" "VIA6522.cpp" "VDU6847.h" "VDU6847.cpp" "PIA8255.h" "PIA8255.cpp" "ROM.h" "ROM.cpp" "RAM.cpp" "RAM.h" "Device.h" "Device.cpp")

# Locate zlib
find_package(ZLIB REQUIRED)

include_directories(
    "${CMAKE_SOURCE_DIR}/shared"
)

# Install information - lib & include
set(installable_libs shared)
install(TARGETS ${installable_libs} DESTINATION lib)
install(
	FILES Codec6502.h P6502.h Device.h PIA8255.h RAM.h ROM.h VDU6847.h VIA6522.h
	DESTINATION include/shared
)
