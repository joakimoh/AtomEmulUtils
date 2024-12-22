add_library(
	shared
	"Codec6502.cpp"
	"Codec6502.h")

# Locate zlib
find_package(ZLIB REQUIRED)

include_directories(
    "${CMAKE_SOURCE_DIR}/shared"
)

# Install information - lib & include
set(installable_libs shared)
install(TARGETS ${installable_libs} DESTINATION lib)
install(
	FILES Codec6502.h
	DESTINATION include/shared
)
