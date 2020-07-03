#include <iostream>


#include "NES.h"

int main(int argc, char* argv[]) {

	VNES::NES nes;
	nes.loadRom("../roms/SuperMarioBros.nes");

	return 0;
}