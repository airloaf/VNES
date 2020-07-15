#include <SDL2/SDL.h>

#include "NES.h"

int main(int argc, char* argv[]) {

	VNES::NES nes;
	nes.loadRom("../roms/DonkeyKong.nes");

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("VNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 960, SDL_WINDOW_SHOWN);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

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
		nes.render(renderer);
		poll++;
		poll %= 30000;
	}

	return 0;
}