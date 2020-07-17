#pragma once

#include <stdint.h>

#include <V6502/CPU.h>
#include <SDL2/SDL.h>

#include "PPURegister.h"
#include "../MemoryBus/PPUBus.h"

namespace VNES{ namespace PPU{

typedef uint8_t Pixel;

struct ScanLine {
	Pixel pixels[256];
};

struct Frame {
	ScanLine scanLines[240];
};

// SDL2 specific stuff
struct PixelData{
	uint8_t a;
	uint8_t b;
	uint8_t g;
	uint8_t r;
};


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

		void render(SDL_Renderer* renderer);

	private:

		uint8_t fetchPatternHigh(uint8_t tileEntry, uint8_t row);
		uint8_t fetchPatternLow(uint8_t tileEntry, uint8_t row);
		uint8_t fetchNametableEntry(uint8_t row, uint8_t col);

		void renderFrame(SDL_Renderer* renderer);
		void renderPatternTable(SDL_Renderer* renderer);

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

		SDL_Texture* mFrame;

		// Pixel data
		PixelData *mPixelData;
		int mPitch[241];
};

}}
