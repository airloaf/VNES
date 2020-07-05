#pragma once

#include <stdint.h>

namespace VNES {namespace PPU {

class NameTable
{
	public:
		/**
		 * @brief read a byte from the nametable
		 * 
		 * @param address - the address to index into the nametable (0x0000 - 0x0400)
		 */
		uint8_t read(uint16_t address);

		/**
		 * @brief write a byte into the nametable
		 * 
		 * @param address - the address to index into the nametable (0x0000 - 0x0400)
		 * @param value - the value to write into the nametable
		 */
		void write(uint16_t address, uint8_t value);

	private:
		uint8_t mData[0x400];

};

}}
