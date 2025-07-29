#include "AddressSpaceInfo.h"


AddressSpace::AddressSpace(uint16_t adr, uint16_t sz) : mAdr(adr), mSz(sz)
{

}

bool AddressSpace::contains(uint16_t adr)
{
	return adr >= mAdr && adr < mAdr + mSz;
}

bool AddressSpace::contains(AddressSpace &space)
{
	return space.mAdr >= mAdr && space.mAdr+space.mSz < mAdr + mSz;
}

bool AddressSpace::intersects(AddressSpace& space)
{
	uint16_t space_low = space.mAdr;
	uint16_t space_high = space_low + space.mSz - 1;
	uint16_t my_space_low = mAdr;
	uint16_t my_space_high = mAdr + mSz - 1;
	return
		space_low >= my_space_low && space_low <= my_space_high ||
		space_high >= my_space_low && space_high <= my_space_high ||
		my_space_low >= space_low && my_space_low <= space_high ||
		my_space_high >= space_low && my_space_high <= space_high;
}

ostream& operator<<(ostream& sout, const AddressSpace& space)
{
	sout << "[0x" << hex << space.mAdr << ",0x" << space.mAdr + space.mSz - 1 << "]";
	return  sout;
}

AddressSpaceInfo::AddressSpaceInfo(uint16_t adr, uint16_t sz)
{
	mSpaces.push_back(AddressSpace(adr, sz));
}

ostream& operator<<(ostream& sout, const AddressSpaceInfo& spaceInfo)
{

	for (int i = 0; i < spaceInfo.mSpaces.size(); i++) {
		if (i > 0)
			sout << ", ";
		sout << spaceInfo.mSpaces[i];
	}
	return  sout;
}

bool AddressSpaceInfo::addGap(uint16_t adr, uint16_t sz)
{
	AddressSpace gap(adr, sz);

	// Check that the gap is not already covered
	if (!contains(gap))
		return true;

	// Split the address space into two pieces around the gap
	AddressSpace space(0, 0);
	bool found_space = false;
	int index = 0;
	for (; index < mSpaces.size(); index++) {
		if (mSpaces[index].contains(gap)) {
			space = mSpaces[index];
			found_space = true;
			break;
		}
	}
	if (!found_space)
		return false;

	// Create two new address spaces
	//
	// +---------------------------------------------+
	// | original space without gaps being specified |
	// +---------------------------------------------+
	// a1				a2      a3					a4
	//                   +------+
	//                   | gap  |
	//                   +------+
	// a1               a2-1    a3+1                a4
	// +-----------------+      +--------------------+
	// | space 1         |      | space 2            |
	// +-----------------+      +--------------------+
	//
	uint16_t a1 = space.getStartOfSpace();
	uint16_t a2 = gap.getStartOfSpace();
	uint16_t a3 = gap.getEndOfSpace();
	uint16_t a4 = space.getEndOfSpace();
	if (a4 < a3 || a3 < a2 || a2 < a1) // Check that the gap is within the original space
		return false;
	AddressSpace space_1(a1, a2 - a1);
	AddressSpace space_2(a3+1, a4 - a3);

	// Replace the old address space element with the two new ones
	mSpaces.erase(mSpaces.begin()+index);
	mSpaces.insert(mSpaces.begin() + index, space_2);
	mSpaces.insert(mSpaces.begin() + index, space_1);

	return true;
}

bool AddressSpaceInfo::operator==(AddressSpaceInfo& memSpace) {

	if (mSpaces.size() != memSpace.mSpaces.size())
		return false;

	for (int i = 0; i < mSpaces.size(); i++) {
		if (mSpaces[i] != memSpace.mSpaces[i])
			return false;
	}

	return true;
}

bool AddressSpaceInfo::operator!=(AddressSpaceInfo& memSpace) {
	return !(*this == memSpace);
}

bool AddressSpaceInfo::contains(uint16_t adr)
{
	if (mSpaces.size() == 1)
		return mSpaces[0].contains(adr);

	for (int i = 0; i < mSpaces.size(); i++) {
		if (mSpaces[i].contains(adr))
			return true;
	}

	return false;
}



bool AddressSpaceInfo::contains(AddressSpace& space)
{
	if (mSpaces.size() == 1)
		return mSpaces[0].contains(space);

	for (int i = 0; i < mSpaces.size(); i++) {
		if (mSpaces[i].contains(space))
			return true;
	}

	return false;
}

bool AddressSpaceInfo::intersects(AddressSpaceInfo& space)
{

	for (int i = 0; i < mSpaces.size(); i++) {
		for (int j = 0; j < space.mSpaces.size(); j++) {
			if (mSpaces[i].intersects(space.mSpaces[j]))
				return true;
		}
	}

	return false;
}