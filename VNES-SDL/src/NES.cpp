#include "NES.h"

#include "Mappers/Mapper0.h"

namespace VNES{

NES::NES(): mMapper(nullptr), mCycle(0), mPPUCycle(0)
{
	// Set the CPU memory bus for the CPU
	mCPU.setMemoryBus(&mCPUBus);

	mCPUBus.setPPU(&mPPU);

	// Set the PPU memory bus for the PPU
	mPPU.setMemoryBus(&mPPUBus);

	mPPU.setCPUReference(&mCPU);


	mWindow = SDL_CreateWindow("NameTable Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800, SDL_WINDOW_SHOWN);
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);

}

NES::~NES()
{
	delete mMapper;
	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);
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
	bool quit = false;
	while(!quit){

		if(mCycle == 0){
			mCPU.tick();
		}
		mPPU.tick();


		// TMP rendering for debugging
		if(mPPUCycle == 0){

			SDL_Event event;
			while(SDL_PollEvent(&event)){
				if(event.type == SDL_QUIT){
					quit = true;
				}
			}

			SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(mRenderer);
			mPPU.renderNameTable(mRenderer);
			SDL_RenderPresent(mRenderer);
		}

		mCycle = (mCycle + 1) % 3;
		mPPUCycle = (mPPUCycle + 1) % (341 * 261);
	}
}

}
