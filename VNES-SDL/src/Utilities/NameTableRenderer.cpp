#include "NameTableRenderer.h"

NameTableRenderer::NameTableRenderer(VNES::NES *nes): mFrameTexture(nullptr), mPixelData(nullptr), mPitch(nullptr){
	mNes = nes;
}

NameTableRenderer::~NameTableRenderer(){
	SDL_DestroyTexture(mFrameTexture);
}

void NameTableRenderer::renderTexture(){
	// Iterate through each of the columns
	for(int tileRow = 0; tileRow < 30 * 2 ; tileRow++){
		for(int tileCol = 0; tileCol < 32 * 2; tileCol++){
			// Fetch tile entry from name table
			uint8_t tileEntry = fetchNameTable(tileRow, tileCol);

			// Iterate through each pixel row of tiles
			for(int row = 0; row < 8; row++){
				int index = tileRow * 64 * 8 * 8 + 64 * 8 * row + (tileCol * 8);

				uint8_t low = fetchTilePatternLow(tileEntry, row);
				uint8_t high = fetchTilePatternHigh(tileEntry, row);

				for(int col = 0; col < 8; col++){
					uint8_t pixel = generatePixel(low, high, col);
					int r, g, b, a;
					r = g = b = 0x00;
					a = 0xFF;
					switch(pixel){
					case 1:
						r = 0xFF;
						break;
					case 2:
						g = 0xFF;
						break;
					case 3:
						b = 0xFF;
						break;
					}
					mPixelData[index + col].r = r;
					mPixelData[index + col].g = g;
					mPixelData[index + col].b = b;
					mPixelData[index + col].a = a;
				}


			}

		}
	}

}

uint8_t NameTableRenderer::fetchAttribute(uint8_t tileRow, uint8_t tileCol){
	if (tileRow >= 30) {
		tileRow -= 30;
	}
	if(tileCol >= 32){
		tileCol -= 32;
	}

	uint16_t colAddr = tileCol / 4;
	uint16_t rowAddr = (tileRow / 4) * 8;
	return 0;
}

uint8_t NameTableRenderer::generatePixel(uint8_t lowBytes, uint8_t highBytes, uint8_t col){

	uint8_t mask = 0x80;
	mask >>= col;
	int restShift = 7 - col;
	int high = (highBytes & mask) >> restShift;
	int low = (lowBytes & mask) >> restShift;

	return low | (high << 1);
}

uint8_t NameTableRenderer::fetchNameTable(uint8_t tileRow, uint8_t tileCol){
	uint16_t vramAddress = 0x2000;

	if(tileCol >= 32){
		vramAddress += 0x400;
		tileCol -= 32;
	}

	if(tileRow >= 30){
		vramAddress += 0x800;
		tileRow -= 30;
	}

	vramAddress += tileRow * 32 + tileCol;
	return mNes->ppuBusRead(vramAddress);
}

uint8_t NameTableRenderer::fetchTilePatternLow(uint8_t tileEntry, uint8_t row){
	uint16_t vramAddress = 0x1000;
	vramAddress += tileEntry * 16 + row + 0;
	return mNes->ppuBusRead(vramAddress);
}

uint8_t NameTableRenderer::fetchTilePatternHigh(uint8_t tileEntry, uint8_t row){
	uint16_t vramAddress = 0x1000;
	vramAddress += tileEntry * 16 + row + 8;
	return mNes->ppuBusRead(vramAddress);
}

void NameTableRenderer::render(SDL_Renderer *renderer){

	if(mFrameTexture == nullptr){
			mFrameTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
				SDL_TEXTUREACCESS_STREAMING, 32*2*8, 30*2*8);
	}

	SDL_LockTexture(mFrameTexture, nullptr, (void**)&mPixelData, (int*)&mPitch);

	renderTexture();

	SDL_UnlockTexture(mFrameTexture);

	SDL_RenderCopy(renderer, mFrameTexture, nullptr, nullptr);

	mPixelData = nullptr;
	mPitch = nullptr;

}
