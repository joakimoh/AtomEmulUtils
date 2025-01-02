#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H

typedef struct DebugInfo_struct {
	bool verbose = false;
	int stopAdr = -1;
	int dumpAdr = -1;
	int dumpSz = -1;
} DebugInfo;


#endif