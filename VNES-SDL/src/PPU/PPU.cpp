#include "PPU.h"

#include <iostream>

namespace VNES {namespace PPU {

	PPU::PPU() : mBus(nullptr), mCPU(nullptr), mFirstAddressWrite(true), mCurrentScanLine(-1), mCurrentCycle(0), mFrame(nullptr)
	{
		mRegisters.PPUSTATUS = 0x80;
	}

	PPU::~PPU()
	{
		SDL_DestroyTexture(mFrame);
	}

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
				mScrollRegisters.nameTable = value & 0x03;
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
					mScrollRegisters.fineX = value & 0x07;
					mScrollRegisters.coarseX = ((value & 0xF8) >> 3);
				}else{
					mScrollRegisters.fineY = value & 0x07;
					mScrollRegisters.coarseY = ((value & 0xF8) >> 3);
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
		handleCycle();

		// Increment the cycles
		mCurrentCycle++;

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
		uint16_t baseNameTable = 0x2000;
		uint16_t nameTableOffset = row * 32 + col;
		return mBus->read(baseNameTable + nameTableOffset);
	}

	void PPU::handleCycle(){
		if(mCurrentScanLine >= 0 && mCurrentScanLine <= 239){
			if(mCurrentCycle >= 1 && mCurrentCycle <= 256){
				int tileCycle = mCurrentCycle - 1;
				if(tileCycle % 8 == 0){
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

					mScanLine.pixels[tileCycle + 0] = 0x00 | high0 | low0;
					mScanLine.pixels[tileCycle + 1] = 0x00 | high1 | low1;
					mScanLine.pixels[tileCycle + 2] = 0x00 | high2 | low2;
					mScanLine.pixels[tileCycle + 3] = 0x00 | high3 | low3;
					mScanLine.pixels[tileCycle + 4] = 0x00 | high4 | low4;
					mScanLine.pixels[tileCycle + 5] = 0x00 | high5 | low5;
					mScanLine.pixels[tileCycle + 6] = 0x00 | high6 | low6;
					mScanLine.pixels[tileCycle + 7] = 0x00 | high7 | low7;
				}
			}
		}
	}

	void PPU::setMemoryBus(MemoryBus::PPUBus* bus)
	{
		mBus = bus;
	}

	void PPU::setCPUReference(V6502::CPU *cpu){
		mCPU = cpu;
	}

	void PPU::renderScanLine(SDL_Renderer *renderer){
		int row = mCurrentScanLine;
		for(int i = 0; i < 256; i++){
			int pallete = mScanLine.pixels[i];
			switch(pallete){
			case 0:
				SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
				break;
			case 1:
				SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
				break;
			case 2:
				SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
				break;
			case 3:
				SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
				break;
			}
			SDL_RenderDrawPoint(renderer, i, row);
		}
	}

	void PPU::renderPatternTable(SDL_Renderer *renderer){
		SDL_SetRenderTarget(renderer, mFrame);
		for (int tileRow = 0; tileRow < 16; tileRow++) {
			for(int tileCol = 0; tileCol < 16; tileCol++){
				for(int row = 0; row < 8; row++){
					uint8_t patternLow = fetchPatternLow(tileRow * 16 + tileCol, row);
					uint8_t patternHigh = fetchPatternHigh(tileRow * 16 + tileCol, row);
				
					uint8_t lowBits[8];
					uint8_t highBits[8];

					lowBits[0] = (patternLow & 0x80) >> 7;
					lowBits[1] = (patternLow & 0x40) >> 6;
					lowBits[2] = (patternLow & 0x20) >> 5;
					lowBits[3] = (patternLow & 0x10) >> 4;
					lowBits[4] = (patternLow & 0x08) >> 3;
					lowBits[5] = (patternLow & 0x04) >> 2;
					lowBits[6] = (patternLow & 0x02) >> 1;
					lowBits[7] = (patternLow & 0x01) >> 0;

					highBits[0] = (patternHigh & 0x80) >> 6;
					highBits[1] = (patternHigh & 0x40) >> 5;
					highBits[2] = (patternHigh & 0x20) >> 4;
					highBits[3] = (patternHigh & 0x10) >> 3;
					highBits[4] = (patternHigh & 0x08) >> 2;
					highBits[5] = (patternHigh & 0x04) >> 1;
					highBits[6] = (patternHigh & 0x02) >> 0;
					highBits[7] = (patternHigh & 0x01) << 1;

					uint8_t palletes[8];
					for (int i = 0; i < 8; i++) {
						palletes[i] = highBits[i] | lowBits[i];
					}

					for(int col = 0; col < 8; col++){
						switch(palletes[col]){
						case 0:
							SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
							break;
						case 1:
							SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
							break;
						case 2:
							SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
							break;
						case 3:
							SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
							break;
						default:
							SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
							break;
						}
						SDL_RenderDrawPoint(renderer, tileCol * 8 + col, tileRow * 8 + row);
					}

				}
			}
		}
	}

	static int frame = 0;

	void PPU::render(SDL_Renderer *renderer){
		if(mFrame == nullptr){
			mFrame = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 256, 240);
		}

		if(mCurrentScanLine < 240 && mCurrentCycle == 00){
			SDL_SetRenderTarget(renderer, mFrame);

			renderScanLine(renderer);
		}

		if(mCurrentScanLine == 240 && mCurrentCycle == 0){
			SDL_SetRenderTarget(renderer, nullptr);
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(renderer);
			//renderPatternTable(renderer);
			//SDL_SetRenderTarget(renderer, nullptr);
			SDL_RenderCopy(renderer, mFrame, nullptr, nullptr);
			SDL_RenderPresent(renderer);
		}
	}

}}
