#pragma once

#include <SDL2/SDL.h>
#include "../NES.h"

class NameTableRenderer
{
	public:
		NameTableRenderer(VNES::NES *nes);
		~NameTableRenderer();

		void render(SDL_Renderer *renderer);

	private:
		void renderTexture();

		uint8_t fetchNameTable(uint8_t tileRow, uint8_t tileCol);

		uint8_t fetchTilePatternLow(uint8_t tileEntry, uint8_t row);
		uint8_t fetchTilePatternHigh(uint8_t tileEntry, uint8_t row);

		uint8_t generatePixel(uint8_t lowBytes, uint8_t highBytes, uint8_t col);

		VNES::NES *mNes;
		
		struct PixelData{
			uint8_t a;
			uint8_t b;
			uint8_t g;
			uint8_t r;
		} *mPixelData;
		
		SDL_Texture *mFrameTexture;
		int *mPitch;

};

