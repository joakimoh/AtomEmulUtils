#include <stdlib.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string.h>
#include "CSWCodec.h"
#include <cmath>
#include "zpipe.h"


using namespace std;


CSWCodec::CSWCodec(int sampleFreq, DebugInfo  *debugInfo): mDebugInfo(debugInfo),mSampleRate(sampleFreq)
{

}

bool CSWCodec::writeSamples(string filePath)
{
    if (mPulses.size() == 0) {
        if (mDebugInfo->dbgLevel & DBG_VERBOSE)
            cout << "Np pulses to write...\n";
        return false;
    }

    if (mDebugInfo->dbgLevel & DBG_VERBOSE)
        cout << "Writing " << mPulses.size() << " samples to file '" << filePath << "...\n";

    // Write samples to CSW file
    CSW2Hdr hdr;
    ofstream fout(filePath, ios::out | ios::binary | ios::ate);

    // Write CSW header
    hdr.csw2.compType = 0x02; // (Z - RLE) - ZLIB compression of data stream
    hdr.csw2.flags = 0; // Initial polarity (specified by bit b0) is LOW
    hdr.csw2.hdrExtLen = 0;
    hdr.csw2.sampleRate[0] = mSampleRate & 0xff;
    hdr.csw2.sampleRate[1] = (mSampleRate >> 8) & 0xff;
    hdr.csw2.sampleRate[2] = (mSampleRate >> 16) & 0xff;
    hdr.csw2.sampleRate[3] = (mSampleRate >> 24) & 0xff;
    int n_pulses = (int)mPulses.size();
    hdr.csw2.totNoPulses[0] = n_pulses & 0xff;
    hdr.csw2.totNoPulses[1] = (n_pulses >> 8) & 0xff;
    hdr.csw2.totNoPulses[2] = (n_pulses >> 16) & 0xff;
    hdr.csw2.totNoPulses[3] = (n_pulses >> 24) & 0xff;
    fout.write((char*)&hdr, sizeof(hdr));

    // Write compressed samples
    if (!encodeBytes(mPulses, fout)) {
        cout << "Failed to compress CSW pulses and write them to file '" << filePath << "'!\n";
        fout.close();
        return false;
    }
    // Add one dummy byte to the end as e.g. CSW viewer seems to read one byte extra (which it shouldn't!)
    char dummy_bytes[] = "0";
    fout.write((char*)&dummy_bytes[0], sizeof(dummy_bytes));

    fout.close();

    // Clear samples to secure that future encodings start without any initial samples
    mPulses.clear();

    return true;
}

bool CSWCodec::decode(string& fileName, vector<uint8_t> &pulses, uint8_t &firstHalfCycleLevel, int &sampleRate)
{
    
    ifstream fin(fileName, ios::in | ios::binary | ios::ate);

    if (!fin) {
        cout << "Failed to open file '" << fileName << "'\n";
        return false;
    }
    
    // Get file size
    fin.seekg(0, ios::end);
    streamsize file_size = fin.tellg();
    
    if (mDebugInfo->dbgLevel & DBG_VERBOSE)
        cout << "CSW file is of size " << dec << file_size << " bytes\n";
   
    // Repositon to start of file
    fin.seekg(0);

    // Read version-independent part of CSW header
    CSWCommonHdr common_hdr;
    if (
        !fin.read((char*)&common_hdr, sizeof(common_hdr)) ||
        common_hdr.terminatorCode != 0x1a ||
        strncmp(common_hdr.signature, "Compressed Square Wave", 22) != 0
        ) {
        cout << "File " << fileName << " is not a valid CSW file (couldn't read a complete header)!\n";
        return false;
    }

    if (!(
        (common_hdr.majorVersion == 0x02 && common_hdr.minorVersion <= 0x01) ||
        (common_hdr.majorVersion == 0x01 && common_hdr.minorVersion == 0x01)
        )) {
        cout << "File " << fileName << " has unsupported format v" << (int)common_hdr.majorVersion << "." << (int)common_hdr.minorVersion << "!\n";
        return false;
    }

    bool compressed;
    int n_pulses;
    string encoding_app = "???";
    bool header_extension = false;
    int n_hdr_ext_bytes = 0;
    if (common_hdr.majorVersion == 0x02) {
        CSW2MainHdr csw2_hdr;
        if (!fin.read((char*)&csw2_hdr, sizeof(csw2_hdr))) {
            cout << "Failed to read complete CSW header of file '" << fileName << "'!\n";
            return false;
        }
        compressed = (csw2_hdr.compType == 0x02);
        mSampleRate = csw2_hdr.sampleRate[0] + (csw2_hdr.sampleRate[1] << 8) + (csw2_hdr.sampleRate[2] << 16) + (csw2_hdr.sampleRate[3] << 24);
        n_pulses = csw2_hdr.totNoPulses[0] + (csw2_hdr.totNoPulses[1] << 8) + (csw2_hdr.totNoPulses[2] << 16) + (csw2_hdr.totNoPulses[3] << 24);
        firstHalfCycleLevel = ((csw2_hdr.flags & 0x01) == 0x01 ? 1 : 0);
        char s[16];
        strncpy(s, csw2_hdr.encodingApp, 16);
        encoding_app = s;

        // Skip extension part
        if (csw2_hdr.hdrExtLen > 0) {
            fin.ignore(csw2_hdr.hdrExtLen);
            header_extension = true;
            n_hdr_ext_bytes = csw2_hdr.hdrExtLen;
        }

    }
    else {
        CSW1MainHdr csw1_hdr;
        if (!fin.read((char*)&csw1_hdr, sizeof(csw1_hdr))) {
            cout << "Failed to read complete CSW header of file '" << fileName << "'!\n";
            return false;
        }
        compressed = false;
        mSampleRate = csw1_hdr.sampleRate[0] + (csw1_hdr.sampleRate[1] << 8);
        n_pulses = -1; // unspecified and therefore undefined for CSW format 1.1
        firstHalfCycleLevel = ((csw1_hdr.flags & 0x01) == 0x01 ? 1 : 0);
    }



    // Assign intial level to pulse (High or Low)
    mPulseLevel = firstHalfCycleLevel;

    if (mDebugInfo->dbgLevel & DBG_VERBOSE)
        cout << "First pulse is " << (int) firstHalfCycleLevel << "\n";


    // Get size of pulse data
    streamsize data_sz = file_size - fin.tellg();

    if (mDebugInfo->dbgLevel & DBG_VERBOSE) {
        cout << "CSW v" << (int)common_hdr.majorVersion << "." << (int)common_hdr.minorVersion << " format with settings:\n";
        cout << "compressed: " << (compressed ? "Z-RLE" : "RLE") << "\n";
        cout << "sample frequency: " << (int)mSampleRate << "\n";
        cout << "no of pulses: " << (int)n_pulses << "\n";
        cout << "initial polarity: " << (int) firstHalfCycleLevel << "\n";
        cout << "encoding app: " << encoding_app << "\n";
        if (header_extension)
            cout << "header extension: " << n_hdr_ext_bytes << " bytes\n";
    }


    //
    // Now read data (i.e. the pulses)
    //


    if (compressed) { // Data is  compressed with ZLIB - need to uncompress it first
        pulses.resize((int)n_pulses);
        if (!decodeBytes(fin, pulses)) {
            cout << "Failed to uncompress CSW data stored in file '" << fileName << "'!\n";
            fin.close();
            return false;
        }
        fin.close();
    }
    else { // Data is not compressed with ZLIB - just read it

        // Collect all pulses into a vector 'pulses'
        pulses.resize((int)data_sz);
        fin.read((char*)&pulses.front(), data_sz);
        fin.close();
    }

    if (mDebugInfo->dbgLevel & DBG_VERBOSE)
        cout << "CSW File '" << fileName << " - no of resulting pulses after decompression = " << pulses.size() << "\n";


    sampleRate = mSampleRate;

    return true;
}

// Tell whether a file is a CSW
bool CSWCodec::isCSWFile(string& fileName)
{
    ifstream fin(fileName, ios::in | ios::binary | ios::ate);

    if (!fin) {
        cout << "Failed to open file '" << fileName << "'\n";
        return false;
    }

    // Reposition to start of file
    fin.seekg(0);

    // Read CSW header
    CSWCommonHdr hdr;
    if (!fin.read((char*)&hdr, (streamsize)sizeof(hdr))) {
        return false;
    }


    if (
        hdr.terminatorCode != 0x1a ||
        strncmp(hdr.signature, "Compressed Square Wave", 22) != 0
        ) {
        return false;
    }


    return true;
}

bool CSWCodec::writePulse(unsigned len)
{
    if (len == 0) {
        cout << "Attempt to write a zero-length pulse!\n";
        return false;
    }

    // Change polarity of the pulse w.r.t previous pulse
    if (mPulseLevel == 0)
        mPulseLevel = 1;
    else
        mPulseLevel = 0;

    if (len <= 255)
        // Pulse can be represented by a single byte
        mPulses.push_back(len);
    else {
        // Pulse needs to be represented by 4 bytes
        mPulses.push_back(0);
        mPulses.push_back(len % 256);
        mPulses.push_back((len >> 8) % 256);
        mPulses.push_back((len >> 16) % 256);
        mPulses.push_back((len >> 24) % 256);
    }



    return true;
}

bool CSWCodec::readPulse(vector<uint8_t> &pulses, int &index, unsigned &len)
{
    if (index < pulses.size() && pulses[index] != 0) {
        len = pulses[index];
        index = index + 1;
    }
    else
    {
        if (index + 4 < pulses.size() && pulses[index] == 0) {
            len = pulses[index + 1];
            len += pulses[index + 2] << 8;
            len += pulses[index + 3] << 16;
            len += pulses[index + 4] << 24;
            index = index + 5;
        }
        else {
            // Error - unexpected termination of pulses
            return false;
        }
    }
    return true;
}

bool CSWCodec::openTapeFileW(string& filePath)
{
    // Clear samples
    mPulses.clear();

    mTapeFilePath = filePath;

    return true;
}

bool CSWCodec::closeTapeFileW()
{
    if (mPulses.size() == 0) {
        if (mDebugInfo->dbgLevel & DBG_VERBOSE)
            cout << "No pulses to write when closing file!\n";
        return false;
    }

    // Write samples to CSW file
    if (!writeSamples(mTapeFilePath)) {
        cout << "Failed to write CSW pulses to file '" << mTapeFilePath << "'!\n";
        return false;
    }

    return true;
}