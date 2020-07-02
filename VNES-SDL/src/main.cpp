#include <iostream>

#include "FileParsers/INesFileParser.h"

int main(int argc, char* argv[]) {
	// Open the Super Mario Bros nes file
	VNES::FileParser::INESFile file;
	file.readFile("../roms/SuperMarioBros.nes");

	if (!file.isValid()) {
		std::cout << "File is not valid!" << std::endl;
	}

	std::cout << "Program Rom Size: " << file.getFileHeader().prgRomSize * (2 << 13) << std::endl;
	std::cout << "Character Rom Size: " << file.getFileHeader().chrRomSize * (2 << 13) << std::endl;

	std::cout << "Mapper: " << file.getFileHeader().mapperNumber << std::endl;
	

	return 0;
}