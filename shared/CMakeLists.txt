add_library(
	shared
	"Codec6502.cpp" "P6502.cpp" "VIA6522.cpp" "VDU6847.cpp" "PIA8255.cpp" "ROM.cpp" "RAM.cpp" "Device.cpp"
	"Keyboard.cpp" "AtomKeyboard.cpp" "ConnectionManager.cpp"
	"Codec6502.h" "P6502.h" "VIA6522.h" "Keyboard.h" "AtomKeyboard.h"
	 "VDU6847.h"  "PIA8255.h"  "ROM.h"  "RAM.h" "Device.h" "ConnectionManager.h" "Connection.h"
 "DebugInfo.h")

# Locate zlib
find_package(ZLIB REQUIRED)

include_directories(
    "${CMAKE_SOURCE_DIR}/shared"
	"${MY_ALLEGRO_SOURCE_DIR}/include"
	"${MY_ALLEGRO_BINARY_DIR}/include"
	}
)

# Install information - lib & include
set(installable_libs shared)
install(TARGETS ${installable_libs} DESTINATION lib)
install(
	FILES Codec6502.h P6502.h Device.h PIA8255.h RAM.h ROM.h VDU6847.h VIA6522.h
	DESTINATION include/shared
)
