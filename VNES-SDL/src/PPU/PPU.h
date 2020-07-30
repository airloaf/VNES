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
		// Scrolling functions
		void fetchNameTable();
		void fetchAttribute();
		void fetchPatternLow();
		void fetchPatternHigh();

		Pixel generatePixel();

		// LOOPY FUNCTIONS
		// http://wiki.nesdev.com/w/index.php/PPU_scrolling
		void loopyIncrementHorizontal();
		void loopyIncrementVertical();
		void loopyCopyHorizontal();
		void loopyCopyVertical();

		void handleCycle();

		// These scroll registers are based off of loopy's scrolling design.
		// V can be thought of as the current address in VRAM.
		// T can be thought of as the temporary address in VRAM or the address
		// of the top left tile to be rendered.
		ScrollRegister mScrollV;
		ScrollRegister mScrollT;

		// The different internal registers for graphics
		uint8_t mCurrentNameTableEntry;
		uint16_t mPatternLowShiftRegister;
		uint16_t mPatternHighShiftRegister;

		PPURegisters mRegisters;

		// PPU bus
		MemoryBus::PPUBus *mBus;

		// Writing to the PPUADDR or PPUSCROLL is different depending if its the first or second write
		bool mFirstAddressWrite;

		// PPU current Scan line and Cycle
		int mCurrentScanLine;
		int mCurrentCycle;

		// Reference to CPU so we can generate NMI requests
		V6502::CPU *mCPU;

		Frame mFrameData;

		// Our renderer instance
		Renderer* mRenderer;
};

}}
