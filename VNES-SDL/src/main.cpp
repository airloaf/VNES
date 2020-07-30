#include <SDL2/SDL.h>

#include "NES.h"

#include "PPU/SDL2Renderer.h"
#include "Utilities/NameTableRenderer.h"

int main(int argc, char* argv[]) {

	VNES::NES nes;
	if(argc == 2){
		nes.loadRom(argv[1]);
	}else{
		nes.loadRom("../roms/DonkeyKong.nes");
	}

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* mainWindow = SDL_CreateWindow("VNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 960, SDL_WINDOW_SHOWN);
	SDL_Renderer *mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
	
	SDL_Window* nameTableWindow = SDL_CreateWindow("NameTableRenderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 960, SDL_WINDOW_SHOWN);
	SDL_Renderer *nameTableRenderer = SDL_CreateRenderer(nameTableWindow, -1, SDL_RENDERER_ACCELERATED);

	SDL2Renderer ppuRenderer;
	nes.setRenderer(&ppuRenderer);

	NameTableRenderer nameTable(&nes);

	bool quit = false;
	int poll = 0;
	while(!quit){
		if(poll == 0){
			SDL_Event event;
			while(SDL_PollEvent(&event)){
				if (event.type == SDL_QUIT) {
					quit = true;
				}
			}
		}

		nes.tick();

		if(ppuRenderer.isReady()){
			SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(mainRenderer);
			
			SDL_SetRenderDrawColor(nameTableRenderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(nameTableRenderer);
			
			ppuRenderer.render(mainRenderer);
			nameTable.render(nameTableRenderer);

			SDL_RenderPresent(mainRenderer);
			SDL_RenderPresent(nameTableRenderer);
		}

		poll++;
		poll %= 300000;
	}

	SDL_DestroyRenderer(nameTableRenderer);
	SDL_DestroyRenderer(mainRenderer);

	SDL_DestroyWindow(nameTableWindow);
	SDL_DestroyWindow(mainWindow);

	SDL_Quit();

	return 0;
}