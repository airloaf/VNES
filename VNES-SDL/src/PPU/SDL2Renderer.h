#pragma once

#include <SDL2/SDL.h>
#include "Renderer.h"

class SDL2Renderer: public VNES::PPU::Renderer
{
	public:

		SDL2Renderer();
		~SDL2Renderer();

		void handleFrame(const VNES::PPU::Frame &f);
		void readyToRender();

		void render(SDL_Renderer *sdlRenderer);

	private:

		void writeFrameToTexture();
	
		struct PixelData{
			uint8_t a;
			uint8_t b;
			uint8_t g;
			uint8_t r;
		} *mPixelData;

		SDL_Texture *mFrameTexture;
		VNES::PPU::Frame mFrameData;

		int *mPitch;

		bool mReady;
};

