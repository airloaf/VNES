#include "NameTableRenderer.h"
static const uint32_t colorTable[] = {
	0x007C7C7C, 0x000000FC, 0x000000BC, 0x004428BC, 0x00940084, 0x00A80020, 0x00A81000, 0x00881400,
	0x00503000, 0x00007800, 0x00006800, 0x00005800, 0x00004058, 0x00000000, 0x00000000, 0x00000000,
	0x00BCBCBC, 0x000078F8, 0x000058F8, 0x006844FC, 0x00D800CC, 0x00E40058, 0x00F83800, 0x00E45C10,
	0x00AC7C00, 0x0000B800, 0x0000A800, 0x0000A844, 0x00008888, 0x00000000, 0x00000000, 0x00000000,
	0x00F8F8F8, 0x003CBCFC, 0x006888FC, 0x009878F8, 0x00F878F8, 0x00F85898, 0x00F87858, 0x00FCA044,
	0x00F8B800, 0x00B8F818, 0x0058D854, 0x0058F898, 0x0000E8D8, 0x00787878, 0x00000000, 0x00000000,
	0x00FCFCFC, 0x00A4E4FC, 0x00B8B8F8, 0x00D8B8F8, 0x00F8B8F8, 0x00F8A4C0, 0x00F0D0B0, 0x00FCE0A8,
	0x00F8D878, 0x00D8F878, 0x00B8F8B8, 0x00B8F8D8, 0x0000FCFC, 0x00F8D8F8, 0x00000000, 0x00000000
};

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
			uint8_t pallete = fetchAttribute(tileRow, tileCol) & 0x03;

			// Iterate through each pixel row of tiles
			for(int row = 0; row < 8; row++){
				int index = tileRow * 64 * 8 * 8 + 64 * 8 * row + (tileCol * 8);

				uint8_t low = fetchTilePatternLow(tileEntry, row);
				uint8_t high = fetchTilePatternHigh(tileEntry, row);

				for(int col = 0; col < 8; col++){
					uint8_t pixel = generatePixel(low, high, col) & 0x03;
					uint16_t palleteAddress = 0x3F00 + pallete * 4 + pixel;
					uint8_t colorIndex = mNes->ppuBusRead(palleteAddress);
					uint32_t color = colorTable[colorIndex];
					int r, g, b, a;
					r = (color & 0x00FF0000) >> 16;
					g = (color & 0x0000FF00) >> 8;
					b = (color & 0x000000FF) >> 0;
					a = 0xFF;

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
	uint16_t vramAddress = 0x23C0;
	if(tileCol >= 32){
		vramAddress += 0x800;
		tileCol -= 32;
	}
	if (tileRow >= 30) {
		vramAddress += 0x400;
		tileRow -= 30;
	}

	// Calculate vram address
	uint16_t colAddr = tileCol / 4;
	uint16_t rowAddr = (tileRow / 4);
	vramAddress += colAddr * 0;
	vramAddress += rowAddr * 8;

	// Read attribute byte
	uint8_t attrByte = mNes->ppuBusRead(vramAddress);

	uint8_t colPos = colAddr % 4;
	uint8_t rowPos = rowAddr % 4;
	
	if(colPos < 2 && rowPos < 2){
		return (attrByte & 0x03) >> 0;
	}else if(colPos < 2 && rowPos >=2){
		return (attrByte & 0x30) >> 4;
	}else if(colPos >= 2 && rowPos < 2){
		return (attrByte & 0x0C) >> 2;
	}else{
		return (attrByte & 0xC0) >> 6;
	}
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
