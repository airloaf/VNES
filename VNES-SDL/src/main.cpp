#include <iostream>


#include "NES.h"

int main(int argc, char* argv[]) {

	VNES::NES nes;
	nes.loadRom("../roms/NesTest.nes");

	nes.tick();

	return 0;
}