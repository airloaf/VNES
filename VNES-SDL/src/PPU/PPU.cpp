#include "PPU.h"

#include <iostream>

namespace VNES {namespace PPU {

	PPU::PPU() : mBus(nullptr), mCPU(nullptr), mFirstAddressWrite(true), mCurrentScanLine(-1), mCurrentCycle(0), mRenderer(nullptr)
	{
		mRegisters.PPUSTATUS = 0x80;
	}

	PPU::~PPU(){}

	uint8_t PPU::cpuRead(uint16_t address)
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
				mRegisters.PPUSCROLL_X = 0x0000;
				mRegisters.PPUSCROLL_Y = 0x0000;
				mFirstAddressWrite = true;
				mFirstScrollWrite = true;
				break;
			case OAMADDR_ADDRESS:
				retValue = mRegisters.OAMADDR;
				break;
			case OAMDATA_ADDRESS:
				retValue = mRegisters.OAMDATA;
				break;
			case PPUSCROLL_ADDRESS:
				retValue = mRegisters.PPUSCROLL_X;
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

	void PPU::cpuWrite(uint16_t address, uint8_t value)
	{
		// Used by PPUADDR
		int addressBitShift = 0;

		switch(address){
			case PPUCTRL_ADDRESS:
				mRegisters.PPUCTRL = value;
				mScrollT.nameTable = value & 0x03;
				mRegisters.useLeftPatternTable = ((value & 0x10) == 0);
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
				if(mFirstScrollWrite){
					mScrollT.fineX = value & 0x07;
					mScrollT.coarseX = ((value & 0xF8) >> 3);
				}else{
					mScrollT.fineY = value & 0x07;
					mScrollT.coarseY = ((value & 0xF8) >> 3);
				}
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
	
	void PPU::tick()
	{
		// Increment the cycles
		mCurrentCycle++;

		handleCycle();

		// Reset NMI after a couple of clock cycles, 6 CPU cycles
		if(mCurrentScanLine == 241 && mCurrentCycle == 18){
			if((mRegisters.PPUCTRL & 0x80) != 0){
				mCPU->setNMI(false);
			}
		}

		// Check if we are done with the scan line
		if(mCurrentCycle > 340){
			// Reset the current cycle
			mCurrentCycle = 0;
			// Increment scan line
			mCurrentScanLine++;

			// If we have finished generating the frame, send it to the renderer
			if(mCurrentScanLine == 240){
				if(mRenderer){
					mRenderer->handleFrame(mFrameData);
					mRenderer->readyToRender();
				}
			}

			// Check if the vblank flag should be set
			if (mCurrentScanLine == 241) {

				// Set vblank flag
				mRegisters.PPUSTATUS |= 0x80;

				// Throw nmi 
				if((mRegisters.PPUCTRL & 0x80) != 0){
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

	void PPU::loopyIncrementHorizontal(){
		// Increment coarse X
		mScrollV.coarseX++;

		// If overflow occurs in coarseX
		// we want to switch to the appropriate nametable
		if((mScrollV.coarseX & 0x20) != 0){
			// TODO: This is ugly
			// Check if we are on the right hand side of nametables
			// if we are, incrememnting the nametable value
			// will put us at the wrong left hand side nametable (0x2000 or 0x2800)
			if((mScrollV.nameTable & 0x01) == 0){
				mScrollV.nameTable++;
			}else{
				mScrollV.nameTable &= 0x02;
			}
		}

		// Strip upper bits
		mScrollV.coarseX &= 0x1F;
	}

	void PPU::loopyIncrementVertical(){

		mScrollV.fineY++;

		// Check if we increment past the tile
		if((mScrollV.fineY & 0x08) != 0){
			mScrollV.coarseY++;

			// Check if coarseY exceeds 29 (30 and 31 is where the attribute table lay)
			if(mScrollV.coarseY > 29){
				mScrollV.coarseY = 0;

				mScrollV.nameTable += 0x02;
				mScrollV.nameTable &= 0x03;
			}

			// Strip upper bits
			mScrollV.coarseY &= 0x1F;
		}

		mScrollV.fineY &= 0x07;

	}

	void PPU::loopyCopyHorizontal(){
		mScrollV.coarseX = mScrollT.coarseX;

		mScrollV.nameTable &= 0x02;
		if((mScrollT.nameTable & 0x01) != 0){
			mScrollV.nameTable |= 0x01;
		}
	}

	void PPU::loopyCopyVertical(){
		mScrollV.fineY = mScrollT.fineY;
		mScrollV.coarseY = mScrollT.coarseY;

		mScrollV.nameTable &= 0x01;
		if((mScrollT.nameTable & 0x02) != 0){
			mScrollV.nameTable |= 0x02;
		}
	}

	void PPU::handleCycle(){

		// Increment horizontal V
		// Cycle must be a multiple of 8 and must be between (0,256) or [328,336]
		if(mCurrentCycle % 8 == 0){
			if((mCurrentCycle > 0 && mCurrentCycle < 256) ||
			(mCurrentCycle >= 328 && mCurrentCycle <= 336)){
				loopyIncrementHorizontal();
			}
		}
		
		if(mCurrentCycle == 256){
			loopyIncrementVertical();
		}

		if(mCurrentCycle == 257){
			loopyCopyHorizontal();
		}

		if(mCurrentScanLine == 261){
			if(mCurrentCycle >= 280 && mCurrentCycle <= 304){
				loopyCopyVertical();
			}
		}


		if(mCurrentScanLine >= 0 && mCurrentScanLine <= 239){
			if(mCurrentCycle >= 1 && mCurrentCycle <= 256){
				int tileCycle = mCurrentCycle - 1;
				if (tileCycle % 8 == 0) {
					// Figure out which tile to render
					uint8_t tileCol = tileCycle / 8;
					uint8_t tileEntry = fetchNametableEntry(mCurrentScanLine / 8, tileCol);

					uint8_t tileLowBitPlane = fetchPatternLow(tileEntry, mCurrentScanLine % 8);
					uint8_t tileHighBitPlane = fetchPatternHigh(tileEntry, mCurrentScanLine % 8);

					uint8_t low0 = (tileLowBitPlane & 0x80) >> 7;
					uint8_t low1 = (tileLowBitPlane & 0x40) >> 6;
					uint8_t low2 = (tileLowBitPlane & 0x20) >> 5;
					uint8_t low3 = (tileLowBitPlane & 0x10) >> 4;
					uint8_t low4 = (tileLowBitPlane & 0x08) >> 3;
					uint8_t low5 = (tileLowBitPlane & 0x04) >> 2;
					uint8_t low6 = (tileLowBitPlane & 0x02) >> 1;
					uint8_t low7 = (tileLowBitPlane & 0x01) >> 0;

					uint8_t high0 = (tileHighBitPlane & 0x80) >> 6;
					uint8_t high1 = (tileHighBitPlane & 0x40) >> 5;
					uint8_t high2 = (tileHighBitPlane & 0x20) >> 4;
					uint8_t high3 = (tileHighBitPlane & 0x10) >> 3;
					uint8_t high4 = (tileHighBitPlane & 0x08) >> 2;
					uint8_t high5 = (tileHighBitPlane & 0x04) >> 1;
					uint8_t high6 = (tileHighBitPlane & 0x02) >> 0;
					uint8_t high7 = (tileHighBitPlane & 0x01) << 1;

					ScanLine& scanLine = mFrameData.scanLines[mCurrentScanLine];
					scanLine.pixels[tileCycle + 0] = 0x00 | high0 | low0;
					scanLine.pixels[tileCycle + 1] = 0x00 | high1 | low1;
					scanLine.pixels[tileCycle + 2] = 0x00 | high2 | low2;
					scanLine.pixels[tileCycle + 3] = 0x00 | high3 | low3;
					scanLine.pixels[tileCycle + 4] = 0x00 | high4 | low4;
					scanLine.pixels[tileCycle + 5] = 0x00 | high5 | low5;
					scanLine.pixels[tileCycle + 6] = 0x00 | high6 | low6;
					scanLine.pixels[tileCycle + 7] = 0x00 | high7 | low7;
				}
			}
		}
	}

	uint8_t PPU::fetchPatternHigh(uint8_t tileEntry, uint8_t row){
		uint16_t tableSection = mRegisters.useLeftPatternTable ? 0x0000 : 0x1000;
		uint16_t patternAddress = tableSection | (tileEntry << 4) | row;
		return mBus->read(patternAddress);
	}

	uint8_t PPU::fetchPatternLow(uint8_t tileEntry, uint8_t row) {
		uint16_t tableSection = mRegisters.useLeftPatternTable ? 0x0000 : 0x1000;
		uint16_t patternAddress = tableSection | (tileEntry << 4) | row + 8;
		return mBus->read(patternAddress);
	}

	uint8_t PPU::fetchNametableEntry(uint8_t row, uint8_t col){
		uint16_t baseNameTable = 0x2000 + mScrollV.nameTable * 0x400;
		uint16_t nameTableOffset = row * 32 + col;
		return mBus->read(baseNameTable + nameTableOffset);
	}

	void PPU::setMemoryBus(MemoryBus::PPUBus* bus)
	{
		mBus = bus;
	}

	void PPU::setCPUReference(V6502::CPU *cpu){
		mCPU = cpu;
	}

	void PPU::setRenderer(Renderer *renderer){
		mRenderer = renderer;
	}

}}
