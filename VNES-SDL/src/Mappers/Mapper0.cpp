#include "Mapper0.h"

namespace VNES { namespace Mapper {

uint8_t Mapper0::cpu_read(uint16_t address)
{
	if (address >= 0x8000 && address < 0xC000) {
		return mFile.getProgramRom()[0x8000 - address];
	}
	if(address >= 0xC000 && address <= 0xFFFF){
		if(mFile.getFileHeader().prgRomSize >= 2){
			return mFile.getProgramRom()[0xC000 - address];
		}else{
			return mFile.getProgramRom()[0x8000 - address];
		}
	}
}
void Mapper0::cpu_write(uint16_t address, uint8_t value)
{
	if (address >= 0x8000 && address < 0xC000) {
		mFile.getProgramRom()[0x8000 - address] = value;
	}
	if(address >= 0xC000 && address <= 0xFFFF){
		if(mFile.getFileHeader().prgRomSize >= 2){
			mFile.getProgramRom()[0xC000 - address] = value;
		}else{
			mFile.getProgramRom()[0x8000 - address] = value;
		}
	}
}
uint8_t Mapper0::ppu_read(uint16_t address)
{
	if(address >= 0x0000 && address <= 0x2000){
		return mFile.getCharacterRom()[address];
	}
}
void Mapper0::ppu_write(uint16_t address, uint8_t value)
{
	if(address >= 0x0000 && address <= 0x2000){
		mFile.getCharacterRom()[address] = value;
	}
}

}}
