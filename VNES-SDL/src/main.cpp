#include <iostream>

#include "NES.h"

int main(int argc, char* argv[]) {

	if(argc > 1){
		VNES::NES nes;
		nes.loadRom(argv[1]);

		while (1) {
			nes.tick();
		}
	}

	return 0;
}