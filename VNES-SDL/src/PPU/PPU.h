#pragma once

#include <stdint.h>

#include <V6502/CPU.h>

#include "PPURegister.h"
#include "Renderer.h"
#include "../MemoryBus/PPUBus.h"

namespace VNES{ namespace PPU{

class PPU
{
	public:

		PPU();
		~PPU();

		uint8_t cpuRead(uint16_t address);
		void cpuWrite(uint16_t address, uint8_t value);

		void setMemoryBus(MemoryBus::PPUBus *bus);
		void setCPUReference(V6502::CPU* cpu);

		void tick();

		void setRenderer(Renderer *renderer);

	private:

		uint8_t fetchPatternHigh(uint8_t tileEntry, uint8_t row);
		uint8_t fetchPatternLow(uint8_t tileEntry, uint8_t row);
		uint8_t fetchNametableEntry(uint8_t row, uint8_t col);

		void handleCycle();

		PPURegisters mRegisters;
		ScrollRegister mScrollRegisters;

		// PPU bus
		MemoryBus::PPUBus *mBus;

		// Writing to the PPUADDR or PPUSCROLL is different depending if its the first or second write
		bool mFirstAddressWrite;
		bool mFirstScrollWrite;

		// PPU current Scan line and Cycle
		int mCurrentScanLine;
		int mCurrentCycle;

		// Reference to CPU so we can generate NMI requests
		V6502::CPU *mCPU;

		Frame mFrameData;

		// Pixel data
		PixelData *mPixelData;
		int mPitch[241];

		// Our renderer instance
		Renderer* mRenderer;
};

}}
