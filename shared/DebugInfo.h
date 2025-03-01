#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H

typedef int DebugLevel;
#define	DBG_NONE			0x000
#define	DBG_ERROR			0x001
#define	DBG_VERBOSE			0x002
#define	DBG_WARNING			0x004
#define	DBG_6502			0x008
#define DBG_PORT			0x010
#define DBG_INTERRUPTS		0x020
#define DBG_KEYBOARD		0x040
#define DBG_VDU				0x080
#define DBG_IO_PERIPHERAL	0x100
#define	DBG_DEVICE			0x200
#define	DBG_ALL				0xfff


typedef struct DebugInfo_struct {
	DebugLevel dbgLevel = DBG_NONE;
	int stopAdr = -1;
	int dumpAdr = -1;
	int dumpSz = -1;
	int traceAdr = -1;
	int postTraceLen = 0;
	int preTraceLen = 10;
	int cyclicLogAdr = -1;
	int interruptLogAdr = -1;
} DebugInfo;




#endif