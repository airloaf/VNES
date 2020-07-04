#include "NES.h"

#include "Mappers/Mapper0.h"

namespace VNES{

NES::NES(): mMapper(nullptr), mCycle(0)
{
	// Set the CPU memory bus for the CPU
	mCPU.setMemoryBus(&mCPUBus);

	mCPUBus.setPPU(&mPPU);

	// Set the PPU memory bus for the PPU
	mPPU.setMemoryBus(&mPPUBus);

	mPPU.setCPUReference(&mCPU);
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

	// Set the mapper in the CPU and PPU
	mCPUBus.setMapper(mMapper);
	mPPUBus.setMapper(mMapper);

}

void NES::tick()
{
	if(mCycle == 0){
		mCPU.tick();
	}
	mPPU.tick();

	mCycle = (mCycle + 1) % 3;
}

}
