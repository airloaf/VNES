#include "Mapper0.h"

namespace VNES { namespace Mapper {

uint8_t Mapper0::cpu_read(uint16_t address)
{
	uint8_t retValue = 0;
	uint8_t* prgRom = mFile->getProgramRom();

	if (address >= 0x8000 && address < 0xC000) {
		retValue = prgRom[address - 0x8000];
	}
	if(address >= 0xC000 && address <= 0xFFFF){
		if(mFile->getFileHeader().prgRomSize >= 2){
			retValue = prgRom[address - 0x8000];
		}else{
			retValue = prgRom[address - 0xC000];
		}
	}

	return retValue;
}
void Mapper0::cpu_write(uint16_t address, uint8_t value)
{
	if (address >= 0x8000 && address < 0xC000) {
		mFile->getProgramRom()[address - 0x8000] = value;
	}
	if(address >= 0xC000 && address <= 0xFFFF){
		if(mFile->getFileHeader().prgRomSize >= 2){
			mFile->getProgramRom()[address - 0xC000] = value;
		}else{
			mFile->getProgramRom()[address - 0x8000] = value;
		}
	}
}
uint8_t Mapper0::ppu_read(uint16_t address)
{
	if(address >= 0x0000 && address <= 0x2000){
		return mFile->getCharacterRom()[address];
	}
}
void Mapper0::ppu_write(uint16_t address, uint8_t value)
{
	if(address >= 0x0000 && address <= 0x2000){
		mFile->getCharacterRom()[address] = value;
	}
}

}}
