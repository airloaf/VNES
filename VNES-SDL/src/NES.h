#pragma once

#include "FileParsers/INesFileParser.h"

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
		VNES::FileParser::INESFile mFile;
};

}
