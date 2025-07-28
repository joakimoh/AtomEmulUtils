add_library(
	shared
	"Codec6502.cpp" "P6502.cpp" "VIA6522.cpp" "VDU6847.cpp" "PIA8255.cpp" "ROM.cpp" "RAM.cpp" "Device.cpp" "TapeRecorder.cpp" "AtomSpeaker.cpp" "VideoDisplayUnit.cpp"
	"AtomKeyboardDevice.cpp" "ConnectionManager.cpp" "Tokeniser.cpp" "MemoryMappedDevice.cpp" "AtomCUTSInterface.cpp" "CSWCodec.cpp" "zpipe.cpp" "GUI.cpp"
	"SoundDevice.cpp" "CRTC6845.cpp" "TT5050.cpp" "BeebRomSel.cpp" "ACIA6850.cpp" "DebugManager.cpp" "BeebSerialULA.cpp" "DeviceManager.cpp"
	"VideoSettings.cpp" "TI4689.cpp" "BeebViaLatch.cpp" "VideoDisplayUnit.cpp" "BeebVideoULA.cpp" "BeebKeyboard.cpp" "Utility.cpp"
	"Codec6502.h" "P6502.h" "VIA6522.h" "AtomKeyboardDevice.h" "MemoryMappedDevice.h" "AtomCUTSInterface.h" "CSWCodec.h" "SoundDevice.h" "BeebVideoULA.h"
	 "VDU6847.h"  "PIA8255.h"  "ROM.h"  "RAM.h" "Device.h" "Tokeniser.h" "DebugManager.h" "GUI.h" "TapeRecorder.h" "AtomSpeaker.h" "VideoDisplayUnit.h"
	"VideoDisplayUnit.h" "SoundDevice.h"  "BeebKeyboard.h" "CRTC6845.h" "TT5050.h"
	"BeebRomSel.h"  "ACIA6850.h" "Utility.h"  "BeebSerialULA.h" "VideoSettings.h" 
	 "ConnectionManager.h"  "DeviceManager.h" "SDCard.cpp" "SDCard.h" "ADC7002.cpp" "ADC7002.h" "Debugger.cpp" "Engine.cpp" "Engine.h" "Display.cpp"  "ProxyMemoryDevice.cpp")

# Locate zlib
find_package(ZLIB REQUIRED)

include_directories(
    "${CMAKE_SOURCE_DIR}/shared"
	"${MY_ALLEGRO_SOURCE_DIR}/include"
	"${MY_ALLEGRO_BINARY_DIR}/include"
	${ZLIB_INCLUDE_DIRS}
)

# Install information - lib & include
set(installable_libs shared)
install(TARGETS ${installable_libs} DESTINATION lib)
install(
	FILES Codec6502.h P6502.h Device.h PIA8255.h RAM.h ROM.h VDU6847.h VIA6522.h zpipe.h CSWCodec.h TapeRecorder.h VideoDataUnit.h Sounddevice.h ADC7002.h
	DESTINATION include/shared
)
