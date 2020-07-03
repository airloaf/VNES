#include "Mapper.h"

namespace VNES { namespace Mapper {

	VNES::Mapper::Mapper::Mapper()
	{
	}

	VNES::Mapper::Mapper::~Mapper()
	{
	}

	void Mapper::loadFile(const FileParser::INESFile& file)
	{
		mFile = file;
	}
	
}}
