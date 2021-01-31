#include "Mapper.h"

namespace VNES { namespace Mapper {

	VNES::Mapper::Mapper::Mapper()
	{
	}

	VNES::Mapper::Mapper::~Mapper()
	{
	}

	void Mapper::loadFile(FileParser::INESFile *file)
	{
		mFile = file;
	}

	FileParser::NametableMirroring Mapper::getMirroring(){
		return mFile->getFileHeader().mirroring;
	}
	
}}
