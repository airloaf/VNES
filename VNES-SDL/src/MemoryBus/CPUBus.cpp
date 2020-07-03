#include "CPUBus.h"

namespace VNES{ namespace MemoryBus {

uint8_t CPUBus::read(uint16_t address)
{
	// Check the address
	if(address >= 0x0000 && address < 0x2000){
		return mInternalRAM[address % 0x800];
	}

	return 0;
}

void CPUBus::write(uint16_t address, uint8_t value)
{
	// Check the address
	if(address >= 0x0000 && address < 0x2000){
		mInternalRAM[address % 0x800] = value;
	}

}

}}
