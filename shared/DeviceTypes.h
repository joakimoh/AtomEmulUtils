#ifndef BUS_TYPES_H
#define BUS_TYPES_H

#include <cstdint>

typedef uint32_t	BusAddress;
typedef uint8_t		BusByte;
typedef uint16_t	BusHalfWord;
typedef uint32_t	BusWord;
typedef uint64_t	BusDoubleWord;
#define BUS_ADDRESS_MASK		0xffff
#define BUS_BYTE_MASK			0xff
#define BUS_HALF_WORD_MASK		0xffff
#define BUS_WORD_MASK			0xffffffff
#define BUS_DOUBOLE_WORD_MASK	0xffffffffffffffff

typedef uint16_t PortVal;
#define PORT_MASK 0xffff
typedef double AnaloguePortVal;

#endif