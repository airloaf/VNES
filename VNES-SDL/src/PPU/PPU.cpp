#include "PPU.h"

#define NAMETABLE_COLS 32
#define NAMETABLE_ROWS 30

#define PATTERNTABLE_COLS 16
#define PATTERNTABLE_ROWS 16
#define BYTES_PER_TILE 16

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
				retValue = 0;
				break;
			case PPUMASK_ADDRESS:
				retValue = 0;
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
				// Increment the address now
				mRegisters.PPUADDR += (mRegisters.PPUCTRL & 0x04) == 0 ? 1 : 32;
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
		PPUMask &mask = mRegisters.mask;

		switch(address){
			case PPUCTRL_ADDRESS:
				mRegisters.PPUCTRL = value;
				mScrollT.nameTable = value & 0x03;
				mRegisters.useLeftPatternTable = ((value & 0x10) == 0);
				break;
			case PPUMASK_ADDRESS:
				mask.greyscale = (value & 0x01) != 0;
				mask.showBorderBackground = (value & 0x02) != 0;
				mask.showBorderSprites = (value & 0x04) != 0;
				mask.showBackground = (value & 0x08) != 0;
				mask.showSprites = (value & 0x10) != 0;
				mask.emphasizeRed = (value & 0x20) != 0;
				mask.emphasizeGreen = (value & 0x40) != 0;
				mask.emphasizeBlue = (value & 0x80) != 0;
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

	bool isVisibleScanLine(int scanLine){
		return scanLine >= 0 && scanLine <= 239;
	}

	bool isPreRenderScanLine(int scanLine){
		return scanLine == 261 || scanLine == -1;
	}

	bool isPostRenderScanLine(int scanLine){
		return scanLine > 239 && scanLine < 261;
	}

	void PPU::handleCycle(){

		// Ignore cycle 0
		if (mCurrentCycle == 0) {
			return;
		}

		if(isVisibleScanLine(mCurrentScanLine) || isPreRenderScanLine(mCurrentScanLine)){
			// Increment horizontal V
			// Cycle must be a multiple of 8 and must be between (0,256) or [328,336]
			if(mCurrentCycle % 8 == 0){
				if((mCurrentCycle > 0 && mCurrentCycle < 256) ||
				(mCurrentCycle == 328 || mCurrentCycle == 336)){
					loopyIncrementHorizontal();
				}
			}
			
			if(mCurrentCycle == 256){
				loopyIncrementVertical();
			}

			if(mCurrentCycle == 257){
				loopyCopyHorizontal();
			}
		}

		if(isPreRenderScanLine(mCurrentScanLine)){
			if(mCurrentCycle >= 280 && mCurrentCycle <= 304){
				loopyCopyVertical();
			}
		}

		if(isVisibleScanLine(mCurrentScanLine) || isPreRenderScanLine(mCurrentScanLine)){
			if((mCurrentCycle >= 1 && mCurrentCycle <= 256) || (mCurrentCycle >= 321 && mCurrentCycle <= 336)){
				// Fetch cycle represents which
				uint8_t fetchCycle = mCurrentCycle - 1;
				switch(fetchCycle % 8){
				case 0:
					fetchNameTable();
					break;
				case 2:
					fetchAttribute();
					break;
				case 4:
					fetchPatternLow();
					break;
				case 6:
					fetchPatternHigh();
					break;
				}

				// Shift the shift registers by 8 bits at scan line 328 since we are loading patterns for next line
				/**/
				if(mCurrentCycle == 328){
					mPatternLowShiftRegister <<= 8;
					mPatternHighShiftRegister <<= 8;
				}
			}
		}
		
		// Compute Pixel
		if(isVisibleScanLine(mCurrentScanLine) && (mCurrentCycle > 0 && mCurrentCycle <= 256)){
			Pixel p = generatePixel();

			ScanLine &s = mFrameData.scanLines[mCurrentScanLine];
			s.pixels[mCurrentCycle - 1] = p;

			mPatternLowShiftRegister <<= 1;
			mPatternHighShiftRegister <<= 1;
		}

	}

	Pixel PPU::generatePixel(){
		if(mCurrentCycle <= 8 && !mRegisters.mask.showBorderBackground){
			return 0;
		}

		uint16_t pixelMask = 0x8000 >> mScrollV.fineX;
		uint16_t restShift = (15 - mScrollV.fineX);
		uint16_t lowBit = (mPatternLowShiftRegister & pixelMask) >> restShift;
		uint16_t highBit = (mPatternHighShiftRegister & pixelMask) >> restShift;

		uint16_t attributeIndex = (highBit << 1) | lowBit;
		Pixel retPixel = attributeIndex;
		return retPixel;
	}

	void PPU::fetchNameTable(){
		uint16_t baseNameTable = 0x2000 + mScrollV.nameTable * 0x400;
		baseNameTable = 0x2000;
		uint16_t nameTableOffset = mScrollV.coarseY * NAMETABLE_COLS + mScrollV.coarseX;
		uint16_t vramAddress = baseNameTable + nameTableOffset;

		uint8_t retValue = mBus->read(vramAddress);
		mCurrentNameTableEntry = retValue;
	}
	
	void PPU::fetchAttribute(){
	}

	uint16_t generatePatternTableAddress(bool leftTable, uint8_t tileEntry){
		uint16_t patternTable = leftTable? 0x0000: 0x1000;
		uint16_t patternTableOffset = tileEntry * BYTES_PER_TILE;
		uint16_t vramAddress = patternTable + patternTableOffset;
		return vramAddress;
	}

	void PPU::fetchPatternLow(){
		bool leftTable = (mRegisters.PPUCTRL & 0x04) == 0;
		uint16_t vramAddress = generatePatternTableAddress(mRegisters.useLeftPatternTable, mCurrentNameTableEntry);

		// Add bytes for the pixel row (controlled by fine y)
		vramAddress += mScrollV.fineY + 0;

		// Read and store bytes into shift register
		uint16_t lowBytes = mBus->read(vramAddress);
		mPatternLowShiftRegister |= (0x00FF & lowBytes);
	}
	
	void PPU::fetchPatternHigh(){
		bool leftTable = (mRegisters.PPUCTRL & 0x04) == 0;
		uint16_t vramAddress = generatePatternTableAddress(mRegisters.useLeftPatternTable, mCurrentNameTableEntry);

		// Add bytes for the pixel row (controlled by fine y)
		// 8 bytes added since high plane is 8 bytes away from low plane
		vramAddress += mScrollV.fineY + 8;

		// Read and store bytes into shift register
		uint16_t highBytes = mBus->read(vramAddress);
		mPatternHighShiftRegister |= (0x00FF & highBytes);
	}

	void PPU::loopyIncrementHorizontal(){
		// Increment coarse X
		mScrollV.coarseX++;

		// If overflow occurs in coarseX
		// we want to switch to the appropriate nametable
		if((mScrollV.coarseX & 0x20) != 0){
			mScrollV.nameTable ^= 0x01;
			mScrollV.coarseX = 0x0;
		}
	}

	void PPU::loopyIncrementVertical(){
		mScrollV.fineY++;

		// Check if we increment past the tile
		if((mScrollV.fineY & 0x08) != 0){
			mScrollV.fineY = 0;
			mScrollV.coarseY++;

			// Check if coarseY exceeds 29 (30 and 31 is where the attribute table lay)
			if(mScrollV.coarseY > 29){
				mScrollV.nameTable ^= 0x02;
				mScrollV.coarseY = 0;
			}
		}
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
