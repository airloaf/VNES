#pragma once

#include "Mapper.h"

namespace VNES { namespace Mapper {

class Mapper0: public Mapper
{
	public:
		uint8_t cpu_read(uint16_t address);

		void cpu_write(uint16_t address, uint8_t value);

		uint8_t ppu_read(uint16_t address);

		void ppu_write(uint16_t address, uint8_t value);

};

}}
