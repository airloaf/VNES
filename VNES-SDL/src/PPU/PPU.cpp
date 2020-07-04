#include "PPU.h"

namespace VNES {namespace PPU {

	PPU::PPU()
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		mWindow = SDL_CreateWindow("PatternTable", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, SDL_WINDOW_SHOWN);
		mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
	}

	PPU::~PPU()
	{
		SDL_DestroyRenderer(mRenderer);
		SDL_DestroyWindow(mWindow);
		SDL_Quit();
	}

	uint8_t PPU::read(uint16_t address)
	{
		switch(address){
			case PPUCTRL_ADDRESS:
				return mRegisters.PPUCTRL;
				break;
			case PPUMASK_ADDRESS:
				return mRegisters.PPUMASK;
				break;
			case PPUSTATUS_ADDRESS:
				return mRegisters.PPUSTATUS;
				break;
			case OAMADDR_ADDRESS:
				return mRegisters.OAMADDR;
				break;
			case OAMDATA_ADDRESS:
				return mRegisters.OAMDATA;
				break;
			case PPUSCROLL_ADDRESS:
				return mRegisters.PPUSCROLL;
				break;
			case PPUADDR_ADDRESS:
				return mRegisters.PPUADDR;
				break;
			case PPUDATA_ADDRESS:
				return mRegisters.PPUDATA;
				break;
			case OAMDMA_ADDRESS:
				return mRegisters.OAMDMA;
				break;
		}
	}

	void PPU::write(uint16_t address, uint8_t value)
	{
		switch(address){
			case PPUCTRL_ADDRESS:
				mRegisters.PPUCTRL = value;
				break;
			case PPUMASK_ADDRESS:
				mRegisters.PPUMASK = value;
				break;
			case PPUSTATUS_ADDRESS:
				mRegisters.PPUSTATUS = value;
				break;
			case OAMADDR_ADDRESS:
				mRegisters.OAMADDR = value;
				break;
			case OAMDATA_ADDRESS:
				mRegisters.OAMDATA = value;
				break;
			case PPUSCROLL_ADDRESS:
				mRegisters.PPUSCROLL = value;
				break;
			case PPUADDR_ADDRESS:
				mRegisters.PPUADDR = value;
				break;
			case PPUDATA_ADDRESS:
				mRegisters.PPUDATA = value;
				break;
			case OAMDMA_ADDRESS:
				mRegisters.OAMDMA = value;
				break;
		}
	}

	void PPU::setMemoryBus(MemoryBus::PPUBus* bus)
	{
		mBus = bus;
	}

	void clearScreen(SDL_Renderer *renderer){
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);
	}
	void setPallete(SDL_Renderer *renderer, int pallete){
		switch(pallete){
		case 0:
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			break;
		case 1:
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
			break;
		case 2:
			SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
			break;
		case 3:
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
			break;
		}
	}

	void PPU::tick(){

		clearScreen(mRenderer);

		int NUM_COLS = 16;
		int NUM_ROWS = 32;

		int PIXELS_PER_TILE_PIXEL = 3;

		int PIXEL_ROWS = 8;
		int PIXEL_COLS = 8;

		uint16_t address = 0x0000;

		// Iterate through each of the tiles
		for(int tileRow = 0; tileRow < NUM_ROWS; tileRow++){
			for(int tileCol = 0; tileCol < NUM_COLS; tileCol++){
			
				uint8_t lsbPlane[8];
				uint8_t msbPlane[8];


				for(int i = 0; i < 8; i++){
					lsbPlane[i] = mBus->read(address++);
				}
				for(int i = 0; i < 8; i++){
					msbPlane[i] = mBus->read(address++);
				}

				for (int row = 0; row < PIXEL_ROWS; row++) {
					uint8_t lsb = lsbPlane[row];
					uint8_t msb = msbPlane[row];
					for (int col = 0; col < PIXEL_COLS; col++) {
						int pallete = 0;
						pallete |= (lsb & 0x80) >> 7;
						pallete |= (msb & 0x80) >> 6;
						lsb <<= 1;
						msb <<= 1;
						setPallete(mRenderer, pallete);


						for (int pixelX = 0; pixelX < PIXELS_PER_TILE_PIXEL; pixelX++) {
							for(int pixelY = 0; pixelY < PIXELS_PER_TILE_PIXEL; pixelY++){
								int x = pixelX + col * PIXELS_PER_TILE_PIXEL + tileCol * PIXEL_COLS * PIXELS_PER_TILE_PIXEL;
								int y = pixelY + row * PIXELS_PER_TILE_PIXEL + tileRow * PIXEL_ROWS * PIXELS_PER_TILE_PIXEL;
								SDL_RenderDrawPoint(mRenderer, x, y);
							}
						}

					}
				}

			}
		}

		SDL_RenderPresent(mRenderer);
		SDL_Delay(200);
	}

}}
