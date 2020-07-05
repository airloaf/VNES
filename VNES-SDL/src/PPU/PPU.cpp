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

	void switchPallete(SDL_Renderer *renderer, int pallete){
		switch (pallete) {
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
	}

	void PPU::renderTile(SDL_Renderer *renderer, uint8_t tileID, int x, int y){
		int PIXELS_PER_TILE = 8;
		int PIXELS_PER_TILE_PIXEL = 3;

		uint8_t lsbPlane[8];
		uint8_t msbPlane[8];

		uint16_t tileAddress = tileID * 16;

		// Read lsb and msb plane
		for(int i = 0; i < 8; i++){
			lsbPlane[i] = mBus->read(tileAddress + i);
			msbPlane[i] = mBus->read(tileAddress + i + 8);
		}

		for(int tileY = 0; tileY < 8; tileY++){
			uint8_t lsb = lsbPlane[tileY];
			uint8_t msb = msbPlane[tileY];
			for(int tileX = 0; tileX < 8; tileX++){
				int pallete = 0;
				pallete |= (lsb & 0x80) >> 7;
				pallete |= (msb & 0x80) >> 6;
				lsb <<= 1;
				msb <<= 1;

				switchPallete(renderer, pallete);

				int drawX = x + tileX * PIXELS_PER_TILE_PIXEL;
				int drawY = y + tileY * PIXELS_PER_TILE_PIXEL;

				for(int i = 0; i < 3; i++){
					for(int i2 = 0; i2 < 3; i2++){
						SDL_RenderDrawPoint(renderer, drawX + i, drawY + i2);
					}
				}

			}
		}

	}

	void PPU::renderNameTable(SDL_Renderer *renderer){

		int PIXELS_PER_TILE = 8;
		int PIXELS_PER_TILE_PIXEL = 3;
		uint16_t baseAddress = 0x2400;

		if(render){
		
			// Render nametable 1
			int NUM_ROWS = 32;
			int NUM_COLS = 32;

			for(int table_row = 0; table_row < NUM_ROWS; table_row++){
				for(int table_col = 0; table_col < NUM_COLS; table_col++){
					uint16_t nameTableAddress = baseAddress + table_col + table_row*NUM_COLS;
					uint8_t tileID = mBus->read(nameTableAddress);
					int x = table_col * PIXELS_PER_TILE * PIXELS_PER_TILE_PIXEL;
					int y = table_row * PIXELS_PER_TILE * PIXELS_PER_TILE_PIXEL;
					renderTile(renderer, tileID, x, y);
				}
			}
		}
	}

}}
