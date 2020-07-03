#include "NES.h"

#include "FileParsers/INesFileParser.h"
#include "MemoryBus/CPUBus.h"
#include "Mappers/Mapper0.h"

#include <iostream>

namespace VNES{

NES::NES()
{
}

NES::~NES()
{
}

void NES::loadRom(const std::string& file_path)
{
	// Load the rom
	mFile.readFile(file_path);

	// Create mapper
	Mapper::Mapper0 mapper0;
	mapper0.loadFile(mFile);

}

void NES::tick()
{
}

}
