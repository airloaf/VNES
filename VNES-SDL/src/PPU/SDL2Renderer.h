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

		SDL_Texture *mFrameTexture;
		VNES::PPU::Frame mFrameData;

		VNES::PPU::PixelData *mPixelData;
		int *mPitch;

		bool mReady;
};

