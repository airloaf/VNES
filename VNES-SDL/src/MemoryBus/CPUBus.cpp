#include "CPUBus.h"

namespace VNES{ namespace MemoryBus {

uint8_t CPUBus::read(uint16_t address)
{

	uint8_t retValue = 0;

	// Check the address
	if(address >= 0x0000 && address < 0x2000){
		retValue = mInternalRAM[address % 0x800];
	}

	if(address >= 0x2000 && address < 0x4000){
		retValue = mPPU->read(0x2000 + (address % 8));
	}

	if(address >= 0x4020){
		retValue = mMapper->cpu_read(address);
	}

	return retValue;
}

void CPUBus::write(uint16_t address, uint8_t value)
{
	// Check the address
	if(address >= 0x0000 && address < 0x2000){
		mInternalRAM[address % 0x800] = value;
	}

	if(address >= 0x2000 && address < 0x4000){
		mPPU->write(0x2000 + (address % 8), value);
	}

	if(address >= 0x4020){
		mMapper->cpu_write(address, value);
	}

}

void CPUBus::setMapper(Mapper::Mapper *mapper){
	mMapper = mapper;
}

void CPUBus::setPPU(PPU::PPU* ppu) {
	mPPU = ppu;
}

}}
