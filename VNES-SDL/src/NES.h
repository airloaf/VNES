#pragma once

#include <V6502/CPU.h>

#include "FileParsers/INesFileParser.h"
#include "MemoryBus/CPUBus.h"

namespace VNES{

/**
 * @brief NES Class
 * Represents the NES as a whole
 */
class NES {

	public:
		NES();
		~NES();

		/**
		 * @brief loads the rom given a file path
		 * 
		 * @param file_path - the path to the rom
		 */
		void loadRom(const std::string &file_path);

		/**
		 * @brief One cycle for the main NES clock
		 */
		void tick();

	private:
		// ROM file
		VNES::FileParser::INESFile mFile;

		// MOS 6502 CPU
		V6502::CPU mCPU;

		// CPU Memory Bus
		MemoryBus::CPUBus mCPUBus;

		// Mapper for the Cartridge
		Mapper::Mapper *mMapper;
};

}
