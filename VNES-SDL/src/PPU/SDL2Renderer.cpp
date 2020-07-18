#include "SDL2Renderer.h"

using namespace VNES::PPU;

SDL2Renderer::SDL2Renderer(): mFrameTexture(nullptr), mPixelData(nullptr), mPitch(nullptr) {}

SDL2Renderer::~SDL2Renderer(){
	SDL_DestroyTexture(mFrameTexture);
}

void SDL2Renderer::handleFrame(const Frame &f){
	mFrameData = f;
}

void SDL2Renderer::readyToRender(){
	mReady = true;
}

void SDL2Renderer::writeFrameToTexture(){
	for(int row = 0; row < 240; row++){
		ScanLine scanLine = mFrameData.scanLines[row];
		for(int col = 0; col < 256; col++){
			int pallete = scanLine.pixels[col];
			PixelData &data = mPixelData[row * 256 + col];
			switch(pallete){
			case 0:
				data.r = 0x00;
				data.g = 0x00;
				data.b = 0x00;
				data.a = 0xFF;
				break;
			case 1:
				data.r = 0xFF;
				data.g = 0x00;
				data.b = 0x00;
				data.a = 0xFF;
				break;
			case 2:
				data.r = 0x00;
				data.g = 0xFF;
				data.b = 0x00;
				data.a = 0xFF;
				break;
			case 3:
				data.r = 0x00;
				data.g = 0x00;
				data.b = 0xFF;
				data.a = 0xFF;
				break;
			}
		}
	}
}

void SDL2Renderer::render(SDL_Renderer* sdlRenderer){
	if(mReady){

		if(!mFrameTexture){
			mFrameTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888,
				SDL_TEXTUREACCESS_STREAMING, 256, 240);
		}

		SDL_SetRenderDrawColor(sdlRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(sdlRenderer);

		SDL_LockTexture(mFrameTexture, nullptr, (void **) &mPixelData, (int *) &mPitch);

		writeFrameToTexture();

		SDL_UnlockTexture(mFrameTexture);
			
		SDL_RenderCopy(sdlRenderer, mFrameTexture, nullptr, nullptr);

		SDL_RenderPresent(sdlRenderer);

		mReady = false;
	}
}
