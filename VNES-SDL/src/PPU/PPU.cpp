#include "PPU.h"

#include <iostream>

namespace VNES {namespace PPU {

	PPU::PPU() : mFirstAddressWrite(true), mCurrentScanLine(-1), mCurrentCycle(0)
	{
		mRegisters.PPUSTATUS = 0x80;
	}

	PPU::~PPU()
	{
	}

	uint8_t PPU::read(uint16_t address)
	{
		uint8_t retValue = 0;
		switch(address){
			case PPUCTRL_ADDRESS:
				retValue = mRegisters.PPUCTRL;
				break;
			case PPUMASK_ADDRESS:
				retValue = mRegisters.PPUMASK;
				break;
			case PPUSTATUS_ADDRESS:
				retValue = mRegisters.PPUSTATUS;

				// Reading the status register will modify many of the bits present within each of these registers
				// Clear bit 7
				mRegisters.PPUSTATUS &= 0x7F;
				// Clear PPU ADDRESS and SCROLL
				mRegisters.PPUADDR = 0x0000;
				mFirstAddressWrite = true;
				break;
			case OAMADDR_ADDRESS:
				retValue = mRegisters.OAMADDR;
				break;
			case OAMDATA_ADDRESS:
				retValue = mRegisters.OAMDATA;
				break;
			case PPUSCROLL_ADDRESS:
				retValue = mRegisters.PPUSCROLL;
				break;
			case PPUADDR_ADDRESS:
				retValue = mRegisters.PPUADDR;
				break;
			case PPUDATA_ADDRESS:
				retValue = mRegisters.PPUDATA;
				break;
			case OAMDMA_ADDRESS:
				retValue = mRegisters.OAMDMA;
				break;
		}

		return retValue;
	}

	void PPU::write(uint16_t address, uint8_t value)
	{
		// Used by PPUADDR
		int addressBitShift = 0;

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
				// Depending on whether or not its the first or last bit, we want to write to the address register differently
				if(mFirstAddressWrite){
					addressBitShift = 8;
					mFirstAddressWrite = false;
				}
				mRegisters.PPUADDR |= (value << addressBitShift);
				break;
			case PPUDATA_ADDRESS:
				// store the value into the register buffer
				mRegisters.PPUDATA = value;
				// Write the value to the address in the PPU bus
				mBus->write(mRegisters.PPUADDR, value);
				// Increment the address now
				mRegisters.PPUADDR += (mRegisters.PPUCTRL & 0x04) == 0 ? 1 : 32;
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

	void PPU::setCPUReference(V6502::CPU *cpu){
		mCPU = cpu;
	}

	void PPU::tick()
	{
		// Increment the cycles
		mCurrentCycle++;

		// Reset NMI after a couple of clock cycles, 20 CPU cycles
		if(mCurrentScanLine == 241 && mCurrentCycle == 60){
			mCPU->setNMI(false);
		}

		// Check if we are done with the scan line
		if(mCurrentCycle > 340){
			// Reset the current cycle
			mCurrentCycle = 0;
			// Increment scan line
			mCurrentScanLine++;

			// Check if the vblank flag should be set
			if (mCurrentScanLine == 241) {

				// Set vblank flag
				mRegisters.PPUSTATUS |= 0x80;

				// Throw nmi 
				if(mRegisters.PPUCTRL & 0x80 != 0){
					mCPU->setNMI(true);
				}

			// Check if we need to clear the vblank flag
			} else if(mCurrentScanLine == 261){
				mRegisters.PPUSTATUS &= 0x7F;

			// Check if we should reset the scan line to 0
			}else if(mCurrentScanLine > 261){

				mCurrentScanLine = 0;
			}
		}

	}

	void PPU::renderNameTable(){

		// Renders the name table
		std::cout << "Start of nametable render" << std::endl;

		

		std::cout << "End of nametable render" << std::endl;
	}

}}
