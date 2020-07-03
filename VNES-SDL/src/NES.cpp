#include "NES.h"

#include "FileParsers/INesFileParser.h"
#include "MemoryBus/CPUBus.h"
#include "Mappers/Mapper0.h"

#include <iostream>

namespace VNES{

NES::NES(): mMapper(nullptr)
{
	// Set the CPU memory bus for the CPU
	mCPU.setMemoryBus(&mCPUBus);

	mCPUBus.setPPU(&mPPU);

}

NES::~NES()
{
	delete mMapper;
}

void NES::loadRom(const std::string& file_path)
{

	// Delete previous mapper if there was any
	delete mMapper;

	// Load the rom
	mFile.readFile(file_path);

	// Create mapper
	mMapper = new Mapper::Mapper0();
	mMapper->loadFile(&mFile);

	// Set the mapper in the CPU
	mCPUBus.setMapper(mMapper);

}

void NES::tick()
{
	mCPU.tick();
}

}
