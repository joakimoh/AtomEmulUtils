#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H

typedef int DebugLevel;
#define	DBG_NONE			0x0
#define	DBG_VERBOSE			0x2
#define	DBG_ERROR			0x1
#define	DBG_WARNING			0x4
#define	DBG_6502			0x8
#define	DBG_DEVICE			0x10
#define	DBG_ALL				0xff


typedef struct DebugInfo_struct {
	DebugLevel dbgLevel = DBG_NONE;
	int stopAdr = -1;
	int dumpAdr = -1;
	int dumpSz = -1;
	int traceAdr = -1;
	int postTraceLen = 0;
	int preTraceLen = 10;
} DebugInfo;




#endif