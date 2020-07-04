#pragma once

#include <stdint.h>

namespace VNES{ namespace PPU{

// PPU has 9 memory mapped addresses, this struct represents those 9 different registers

#define PPUCTRL_ADDRESS		0x2000
#define PPUMASK_ADDRESS		0x2001
#define PPUSTATUS_ADDRESS	0x2002
#define OAMADDR_ADDRESS		0x2003
#define OAMDATA_ADDRESS		0x2004
#define PPUSCROLL_ADDRESS	0x2005
#define PPUADDR_ADDRESS		0x2006
#define PPUDATA_ADDRESS		0x2007
#define OAMDMA_ADDRESS		0x4014

/**
 * The 9 registers of the PPU
 */
struct PPURegisters{
	uint8_t PPUCTRL;	// 0x2000
	uint8_t PPUMASK;	// 0x2001
	uint8_t PPUSTATUS;	// 0x2002
	uint8_t OAMADDR;	// 0x2003
	uint8_t OAMDATA;	// 0x2004
	uint8_t PPUSCROLL;	// 0x2005
	uint16_t PPUADDR;	// 0x2006
	uint8_t PPUDATA;	// 0x2007
	uint8_t OAMDMA;	// 0x4014

};

}}
