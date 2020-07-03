#include <iostream>


#include "NES.h"

int main(int argc, char* argv[]) {

	VNES::NES nes;
	nes.loadRom("../roms/SuperMarioBros.nes");

	while (1) {
		nes.tick();
	}

	return 0;
}