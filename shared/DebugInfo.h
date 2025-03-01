#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H

typedef int DebugLevel;
#define	DBG_NONE			0x00
#define	DBG_ERROR			0x01
#define	DBG_VERBOSE			0x02
#define	DBG_WARNING			0x04
#define	DBG_6502			0x08
#define DBG_PORT			0x10
#define DBG_INTERRUPTS		0x20
#define	DBG_DEVICE			0x40
#define	DBG_ALL				0xff


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