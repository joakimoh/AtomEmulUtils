#ifndef ADDRESS_SPACE_INFO_H
#define ADDRESS_SPACE_INFO_H

#include <vector>
#include <iostream>
#include <cstdint>

using namespace std;

class AddressSpace {

	uint16_t mAdr;
	uint16_t mSz;

public:

	AddressSpace(uint16_t adr, uint16_t sz);

	bool operator==(AddressSpace& space) {
		return mAdr == space.mAdr && mSz == space.mSz;
	}
	bool operator!=(AddressSpace& space) {
		return mAdr != space.mAdr || mSz != space.mSz;
	}

	friend ostream& operator<<(ostream& sout, const AddressSpace& space);

	bool contains(uint16_t adr);
	bool contains(AddressSpace& space);
	bool intersects(AddressSpace& space);

	uint16_t getStartOfSpace() { return mAdr; }
	uint16_t getSizeOfSpace() { return mSz; }
	uint16_t getEndOfSpace() { return mAdr + mSz - 1; }
};

class AddressSpaceInfo {

private:
	vector<AddressSpace> mSpaces; // only one element unless the claimed space is broken up in non-continous fragements

public:

	AddressSpaceInfo(uint16_t adr, uint16_t sz);

	bool addGap(uint16_t adr, uint16_t sz);

	bool operator==(AddressSpaceInfo& memSpace);
	bool operator!=(AddressSpaceInfo& memSpace);

	friend ostream& operator<<(ostream& sout, const AddressSpaceInfo& space);

	bool contains(uint16_t adr);
	bool contains(AddressSpace& space);
	bool intersects(AddressSpaceInfo& space);

	bool hasGaps() { return mSpaces.size() != 1; }
	uint16_t getStartOfSpace() { return mSpaces[0].getStartOfSpace(); }
	uint16_t getEndOfSpace() { return mSpaces[mSpaces.size()-1].getEndOfSpace(); }
	uint16_t getSizeOfSpace() { return getEndOfSpace() - getStartOfSpace() + 1; }
	vector<AddressSpace> getAddressSpaces() { return mSpaces; }
};

#endif
