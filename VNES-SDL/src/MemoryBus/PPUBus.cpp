#include "PPUBus.h"

namespace VNES {namespace MemoryBus {


	PPUBus::PPUBus() : mMapper(nullptr)
	{
	}

	PPUBus::~PPUBus()
	{
	}

	uint8_t PPUBus::read(uint16_t address)
	{
		uint8_t retValue = 0;
		if(address >= 0x0000 && address < 0x2000){
			// Read from the pattern table
			retValue = mMapper->ppu_read(address);
		}

		return retValue;
	}

	void PPUBus::write(uint16_t address, uint8_t value)
	{
		if(address >= 0x0000 && address < 0x2000){
			// Read from the pattern table
			mMapper->ppu_write(address, value);
		}
	}

	void PPUBus::setMapper(Mapper::Mapper *mapper){
		mMapper = mapper;
	}

}}
