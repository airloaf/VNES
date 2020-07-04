#include "PPU.h"

namespace VNES {namespace PPU {

	PPU::PPU()
	{
	}

	PPU::~PPU()
	{
	}

	uint8_t PPU::read(uint16_t address)
	{
		switch(address){
			case PPUCTRL_ADDRESS:
				return mRegisters.PPUCTRL;
				break;
			case PPUMASK_ADDRESS:
				return mRegisters.PPUMASK;
				break;
			case PPUSTATUS_ADDRESS:
				return mRegisters.PPUSTATUS;
				break;
			case OAMADDR_ADDRESS:
				return mRegisters.OAMADDR;
				break;
			case OAMDATA_ADDRESS:
				return mRegisters.OAMDATA;
				break;
			case PPUSCROLL_ADDRESS:
				return mRegisters.PPUSCROLL;
				break;
			case PPUADDR_ADDRESS:
				return mRegisters.PPUADDR;
				break;
			case PPUDATA_ADDRESS:
				return mRegisters.PPUDATA;
				break;
			case OAMDMA_ADDRESS:
				return mRegisters.OAMDMA;
				break;
		}
	}

	void PPU::write(uint16_t address, uint8_t value)
	{
		switch(address){
			case PPUCTRL_ADDRESS:
				mRegisters.PPUCTRL = value;
				break;
			case PPUMASK_ADDRESS:
				mRegisters.PPUMASK = value;
				break;
			case PPUSTATUS_ADDRESS:
				mRegisters.PPUSTATUS = value;
				break;
			case OAMADDR_ADDRESS:
				mRegisters.OAMADDR = value;
				break;
			case OAMDATA_ADDRESS:
				mRegisters.OAMDATA = value;
				break;
			case PPUSCROLL_ADDRESS:
				mRegisters.PPUSCROLL = value;
				break;
			case PPUADDR_ADDRESS:
				mRegisters.PPUADDR = value;
				break;
			case PPUDATA_ADDRESS:
				mRegisters.PPUDATA = value;
				break;
			case OAMDMA_ADDRESS:
				mRegisters.OAMDMA = value;
				break;
		}
	}

	void PPU::setMemoryBus(MemoryBus::PPUBus* bus)
	{
		mBus = bus;
	}

}}
