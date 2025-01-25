#ifndef CSW_CODEC_H
#define CSW_CODEC_H


#include <vector>
#include <string>
#include <cstdint>
#include "DebugInfo.h"

using namespace std;

class CSWCodec
{
	// Default values used: 1f 8b 08 00 00 00
	typedef struct CompressedFileHdr_struct {
		uint8_t CmF = 0x1f;
		uint8_t Flg = 0x8b;
		uint8_t DictId[4] = { 0x08, 0x00, 0x00, 0x00 };
	} CompressedFileHdr;

	// Version-independent part of CSW header
	typedef struct CSWCommonHdr_struct {
		char signature[22]; // "Compressed Square Wave"
		uint8_t terminatorCode = 0x1a;
		uint8_t majorVersion;
		uint8_t minorVersion;
	} CSWCommonHdr;

	//
	// CSW2 format (v2.0) used by most recent tools
	//

	// CSW2-unique header part

	typedef struct CSW2MainHdr_struct {
		uint8_t sampleRate[4]; // sample rate (little-endian)
		uint8_t totNoPulses[4]; // #pulses (little-endian)
		uint8_t compType = 0x01; // 0x01 (RLE) - no compression; 0x02 (Z-RLE) - ZLIB compression ONLY of data stream
		uint8_t flags; // b0: initial polarity; if set, the signal starts at logical high
		uint8_t hdrExtLen; // For future expansions only
		char encodingApp[16] = { 'A','t','o','m','t','a','p','e','U','t','i','l','s' };
	} CSW2MainHdr;

	//
	// CSW Format
	//
	// Specifies pulses that corresponds to the duration (in # samples) of a LOW or HIGH half_cycle
	//
	// --  PULSE #1 --|-- PULSE #2 ---
	// (8 samples)    | (5 samples)
	// x x x x x x x x
	//                 x x x x x
	//

	// The complete CSW2 header
	typedef struct CSW2Hdr_struct {
		CSWCommonHdr common = { {'C','o','m','p','r','e','s','s','e','d',' ','S','q','u','a','r','e',' ','W','a','v','e'} , 0x1a, 0x02, 0x00 };
		CSW2MainHdr csw2;
		// uint8_t hdrExtData[hdrExtLen]; // For future expansions only
		// uint8_t data[totNoPulses];
	} CSW2Hdr;


	//
	// Legacy CSW format (v1.1) used by some tools
	//

	// CSW1-unique header part
	typedef struct CSW1MainHdr_struct {
		uint8_t sampleRate[2]; // sample rate (little-endian)
		uint8_t compType = 0x01; // 0x01 (RLE) - no compression; 0x02 (Z-RLE) - ZLIB compression ONLY of data stream
		uint8_t flags; // b0: initial polarity; if set, the signal starts at logical high
		uint8_t reserved[3]; // Reserved
	} CSW1MainHdr;

	// The complete CSW1 header
	typedef struct CSW1Hdr_struct {
		CSWCommonHdr common = { {'C','o','m','p','r','e','s','s','e','d',' ','S','q','u','a','r','e',' ','W','a','v','e'} , 0x1a, 0x01, 0x01 };
		CSW1MainHdr csw1;
		// uint8_t data[];

	} CSW1Hdr;

private:

	int mSampleRate;

	DebugInfo mDebugInfo;

	string mTapeFilePath;

	// Current pulse level (writing)
	uint8_t mPulseLevel;

	// Pulses read or to write
	vector<uint8_t> mPulses;

public:

	// Default constructor
	CSWCodec(int sampleFreq, DebugInfo debugInfo);

	// Decode a CSW file as a vector of pulses
	bool decode(string &fileName, vector<uint8_t> &pulses, uint8_t &firstHalfCycleLevel, int &sampleRate);

	// Tell whether a file is a CSW file
	static bool isCSWFile(string& CSWFileName);

	bool writeSamples(string filePath);

	bool writePulse(unsigned len);
	bool readPulse(vector<uint8_t> &pulses, int& index, unsigned& len);

	// Open CSW file for writing
	bool openTapeFileW(string& filePath);

	// Close CSW file that was written to
	bool closeTapeFileW();




};

#endif