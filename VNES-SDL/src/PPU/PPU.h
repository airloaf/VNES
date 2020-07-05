#pragma once

#include <stdint.h>

#include <V6502/CPU.h>
#include <SDL2/SDL.h>

#include "PPURegister.h"
#include "../MemoryBus/PPUBus.h"

namespace VNES{ namespace PPU{

class PPU
{
	public:

		PPU();
		~PPU();

		/**
		 * @brief Read from the given address into the PPU
		 * 
		 * The PPU can be read from addresses 0x2000 - 0x2007,
		 * repeated from 0x2008 - 0x3FFF. Also address 0x4014
		 * can be addressed to write OAM data quickly.
		 * 
		 * @param address - the address to read into the PPU
		 * @return 
		 */
		uint8_t read(uint16_t address);

		/**
		 * @brief Write to the PPU at the given address
		 * 
		 * The PPU can be written to from addresses 0x2000 - 0x2007,
		 * repeated from 0x2008 - 0x3FFF. Also address 0x4014
		 * can be addressed to write OAM data quickly.
		 * 
		 * @param address - the address to write into the PPU address
		 * @param value - the value to write
		 */
		void write(uint16_t address, uint8_t value);

		/**
		 * @brief PPU bus reference
		 * 
		 * @param bus - the ppu bus reference
		 */
		void setMemoryBus(MemoryBus::PPUBus *bus);

		/**
		 * @brief set CPU reference
		 * 
		 * @param cpu - the cpu reference
		 */
		void setCPUReference(V6502::CPU* cpu);

		/**
		 * @brief PPU clock cycle function
		 */
		void tick();

		void renderNameTable(SDL_Renderer *renderer);

	private:

		void renderTile(SDL_Renderer* renderer, uint8_t tileID, int x, int y);

		// The PPU registers (0x2000 - 0x2007 and 0x4014)
		PPURegisters mRegisters;

		// PPU bus
		MemoryBus::PPUBus *mBus;

		// Writing to the PPUADDR is different depending if its the first or second write
		bool mFirstAddressWrite;

		// PPU current Scan line and Cycle
		int mCurrentScanLine;
		int mCurrentCycle;

		// Reference to CPU so we can generate NMI requests
		V6502::CPU *mCPU;

	bool render = true;

};

}}
