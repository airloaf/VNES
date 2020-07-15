#include "NES.h"

#include "Mappers/Mapper0.h"

namespace VNES{

NES::NES(): mMapper(nullptr), mCycle(0)
{
	mCPU.setMemoryBus(&mCPUBus);
	mCPUBus.setPPU(&mPPU);

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


	// Run for 6 cycles on cpu startup
	for (int i = 0; i < 6; i++) {
		mCPU.tick();
	}

	V6502::RegisterFile rf = mCPU.getRegisterFile();
	rf.status = 0x34;
	rf.accumulator = 0;
	rf.indexX = 0;
	rf.indexY = 0;
	mCPUBus.write(0x4017, 0x00);
	mCPUBus.write(0x4015, 0x00);
	for (int i = 0x4000; i <= 0x400F; i++) {
		mCPUBus.write(i, 0x00);
	}
	mCPUBus.write(0x4010, 0x00);
	mCPUBus.write(0x4011, 0x00);
	mCPUBus.write(0x4012, 0x00);
	mCPUBus.write(0x4013, 0x00);

	mPPUBus.write(0x2000, 0x00);
	mPPUBus.write(0x2001, 0x00);
	mPPUBus.write(0x2002, 0xA0);
	mPPUBus.write(0x2003, 0x00);
	mPPUBus.write(0x2005, 0x00);
	mPPUBus.write(0x2006, 0x00);
}

void NES::tick()
{
	if(mCycle == 0){
		mCPU.tick();
	}
	mPPU.tick();

	mCycle = (mCycle + 1) % 1;
}

void NES::render(SDL_Renderer *renderer){
	mPPU.render(renderer);
	SDL_SetRenderTarget(renderer, nullptr);
}

}
