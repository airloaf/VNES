#include "NameTable.h"


namespace VNES{ namespace PPU{

uint8_t NameTable::read(uint16_t address)
{
	return mData[address];
}

void NameTable::write(uint16_t address, uint8_t value)
{
	mData[address] = value;
}

}}
