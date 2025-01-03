#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H

typedef struct DebugInfo_struct {
	bool verbose = false;
	int stopAdr = -1;
	int dumpAdr = -1;
	int dumpSz = -1;
	int traceAdr = -1;
	int postTraceLen = 0;
	int preTraceLen = 10;
} DebugInfo;


#endif