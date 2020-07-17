#pragma once

#include <V6502/MemoryBus.h>
#include "../Mappers/Mapper.h"
#include "../PPU/NameTable.h"

namespace VNES{ namespace MemoryBus{

class PPUBus: public V6502::MemoryBus
{

	public:

		PPUBus();
		~PPUBus();

		/**
		 * @brief Reads from the PPU bus given the address
		 * 
		 * @param address - the address to read from the PPU bus
		 * @return 
		 */
		uint8_t read(uint16_t address);

		/**
		 * @brief Write to the PPU bus at the given address and value
		 * 
		 * @param address - the address to write at the bus
		 * @param value - the value to write into the bus
		 */
		void write(uint16_t address, uint8_t value);

		void setMapper(Mapper::Mapper *mapper);

	private:
		Mapper::Mapper* mMapper;
		PPU::NameTable mNameTables[4];
		uint8_t mPalletes[0x20];
};

}}
