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

		if(address >= 0x2000 && address < 0x3EFF){
			if(address > 0x3000){
				address = 0x2000 + (address - 0x3000);
			}

			int index = 0;
			if(address >= 0x2400 && address < 0x2800){
				index = 1;
			}else if(address >= 0x2800 && address < 0x2C00){
				index = 2;
			}else if(address >= 0x2C00 && address < 0x3000){
				index = 3;
			}
			retValue = mNameTables[index].read(address);
		}

		return retValue;
	}

	void PPUBus::write(uint16_t address, uint8_t value)
	{
		if(address >= 0x0000 && address < 0x2000){
			// Read from the pattern table
			mMapper->ppu_write(address, value);
		}

		if (address >= 0x2000 && address < 0x3EFF) {
			if (address >= 0x3000) {
				address = 0x2000 + (address - 0x3000);
			}

			int index = 0;
			if(address >= 0x2000 && address < 0x2400){
				index = 0;
				address -= 0x2000;
			}else if (address >= 0x2400 && address < 0x2800) {
				index = 1;
				address -= 0x2400;
			}else if(address >= 0x2800 && address < 0x2C00){
				index = 2;
				address -= 0x2800;
			}else if(address >= 0x2C00 && address < 0x3000){
				index = 3;
				address -= 0x2C00;
			}
			mNameTables[index].write(address, value);
		}
	}

	void PPUBus::setMapper(Mapper::Mapper *mapper){
		mMapper = mapper;
	}

}}
