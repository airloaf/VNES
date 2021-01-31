#include "PPUBus.h"

namespace VNES {namespace MemoryBus {


	PPUBus::PPUBus() : mMapper(nullptr){}

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

		if (address >= 0x2000 && address <= 0x3EFF) {
			if (address >= 0x3000) {
				address = 0x2000 + (address - 0x3000);
			}

			int index = 0;
			if(address >= 0x2000 && address < 0x2400){
				address -= 0x2000;
				index = 0;
			}else if (address >= 0x2400 && address < 0x2800) {
				address -= 0x2400;
				if(mMapper->getMirroring() == FileParser::NametableMirroring::HORIZONTAL){
					index = 0;
				}else {
					index = 1;
				}
			}else if(address >= 0x2800 && address < 0x2C00){
				address -= 0x2800;
				if(mMapper->getMirroring() == FileParser::NametableMirroring::HORIZONTAL){
					index = 1;
				}else {
					index = 0;
				}
			}else if(address >= 0x2C00 && address < 0x3000){
				address -= 0x2C00;
				index = 1;
			}
			retValue = mNameTables[index].read(address);
		}

		if(address >= 0x3F00 && address < 0x4000){
			address &= 0x001F;
			if(address % 4 == 0){
				retValue = mPalletes[0];
			}else{
				retValue = mPalletes[address];
			}
		}

		return retValue;
	}

	void PPUBus::write(uint16_t address, uint8_t value)
	{
		if(address >= 0x0000 && address < 0x2000){
			// Read from the pattern table
			mMapper->ppu_write(address, value);
		}

		if (address >= 0x2000 && address <= 0x3EFF) {
			if (address >= 0x3000) {
				address = 0x2000 + (address - 0x3000);
			}

			int index = -1;
			if(address >= 0x2000 && address < 0x2400){
				address -= 0x2000;
				index = 0;
			}else if (address >= 0x2400 && address < 0x2800) {
				address -= 0x2400;
				if(mMapper->getMirroring() == FileParser::NametableMirroring::HORIZONTAL){
					index = 0;
				}else {
					index = 1;
				}
			}else if(address >= 0x2800 && address < 0x2C00){
				address -= 0x2800;
				if(mMapper->getMirroring() == FileParser::NametableMirroring::HORIZONTAL){
					index = 1;
				}else {
					index = 0;
				}
			}else if(address >= 0x2C00 && address < 0x3000){
				address -= 0x2C00;
				index = 1;
			}
			mNameTables[index].write(address, value);

		}
		
		if(address >= 0x3F00 && address < 0x4000){
			address &= 0x001F;
			if(address % 4 == 0){
				mPalletes[0] = value;
			}else{
				mPalletes[address] = value;
			}
		}
	}

	void PPUBus::setMapper(Mapper::Mapper *mapper){
		mMapper = mapper;
	}

}}
