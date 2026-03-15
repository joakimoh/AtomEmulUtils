#ifndef ADDRESS_SPACE_INFO_H
#define ADDRESS_SPACE_INFO_H

#include <vector>
#include <iostream>
#include <cstdint>
#include "DeviceTypes.h"

using namespace std;

class AddressSpace {

	BusAddress mAdr;
	BusAddress mSz;

public:

	AddressSpace(BusAddress adr, BusAddress sz);

	bool operator==(AddressSpace& space) {
		return mAdr == space.mAdr && mSz == space.mSz;
	}
	bool operator!=(AddressSpace& space) {
		return mAdr != space.mAdr || mSz != space.mSz;
	}

	friend ostream& operator<<(ostream& sout, const AddressSpace& space);
	friend string& operator+(string& sout, const AddressSpace& space);


	bool contains(BusAddress adr);
	bool contains(AddressSpace& space);
	bool intersects(AddressSpace& space);

	BusAddress getStartOfSpace() { return mAdr; }
	BusAddress getSizeOfSpace() { return mSz; }
	BusAddress getEndOfSpace() { return mAdr + mSz - 1; }
};

class AddressSpaceInfo {

private:
	vector<AddressSpace> mSpaces; // only one element unless the claimed space is broken up in non-continous fragements

public:

	AddressSpaceInfo(BusAddress adr, BusAddress sz);

	bool addGap(BusAddress adr, BusAddress sz);

	bool operator==(AddressSpaceInfo& memSpace);
	bool operator!=(AddressSpaceInfo& memSpace);

	friend ostream& operator<<(ostream& sout, const AddressSpaceInfo& spaceInfo);
	friend string& operator+(string& sout, const AddressSpaceInfo& spaceInfo);

	bool contains(BusAddress adr);
	bool contains(AddressSpace& space);
	bool intersects(AddressSpaceInfo& space);

	bool hasGaps() { return mSpaces.size() != 1; }
	BusAddress getStartOfSpace() { return mSpaces[0].getStartOfSpace(); }
	BusAddress getEndOfSpace() { return mSpaces[mSpaces.size()-1].getEndOfSpace(); }
	BusAddress getSizeOfSpace() { return getEndOfSpace() - getStartOfSpace() + 1; }
	vector<AddressSpace> getAddressSpaces() { return mSpaces; }
};

#endif
