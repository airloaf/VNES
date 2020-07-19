#pragma once

#include "../FileParsers/INesFileParser.h"

namespace VNES { namespace Mapper {

class Mapper
{
	public:

		Mapper();
		~Mapper();

		/**
		 * @brief Loads an INES file into a mapper
		 * 
		 * @param file - the file to use in the mapper
		 */
		void loadFile(FileParser::INESFile *file);

		/**
		 * @brief Reads from the correct cpu address using the mapper
		 * 
		 * @param address - the address to read from the mapper
		 * @return 
		 */
		virtual uint8_t cpu_read(uint16_t address) = 0;

		/**
		 * @brief Writes to the correct cpu address using the mapper
		 * 
		 * @param address - the address to write at the mapper
		 * @param value - the value to write into the mapper
		 */
		virtual void cpu_write(uint16_t address, uint8_t value) = 0;
		
		/**
		 * @brief Reads from the correct ppu address using the mapper
		 * 
		 * @param address - the address to read from the mapper
		 * @return 
		 */
		virtual uint8_t ppu_read(uint16_t address) = 0;

		/**
		 * @brief Writes to the correct ppu address using the mapper
		 * 
		 * @param address - the address to write at the mapper
		 * @param value - the value to write into the mapper
		 */
		virtual void ppu_write(uint16_t address, uint8_t value) = 0;

		FileParser::NametableMirroring getMirroring();

	protected:
		FileParser::INESFile *mFile;

};

}}
