#pragma once

#include <V6502/MemoryBus.h>

namespace VNES { namespace MemoryBus {


/**
 * @brief CPU bus for the NES
 */
class CPUBus: public V6502::MemoryBus
{
	public:

		/**
		 * @brief Reads from the CPU bus given the address
		 * 
		 * @param address - the address to read from the CPU bus
		 * @return 
		 */
		uint8_t read(uint16_t address);

		/**
		 * @brief Write to the CPU bus at the given address and value
		 * 
		 * @param address - the address to write at the bus
		 * @param value - the value to write into the bus
		 */
		void write(uint16_t address, uint8_t value);

	private:
		// Internal Ram (0x0000 - 0x2000)
		uint8_t mInternalRAM[0x800];
		

};

}}
