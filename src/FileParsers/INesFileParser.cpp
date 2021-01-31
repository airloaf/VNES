#include "INesFileParser.h"

#include <fstream>

namespace VNES{ namespace FileParser {

    INESFile::INESFile(): mIsValidFile(false), mPrgRomData(nullptr), mChrRomData(nullptr), mTrainerData(nullptr), mPlayChoiceINSTData(nullptr), mPlayChoicePROMData(nullptr) {}

    INESFile::~INESFile(){
        delete mPrgRomData;
        delete mChrRomData;
        delete mTrainerData;
        delete mPlayChoiceINSTData;
        delete mPlayChoicePROMData;
    }

    void INESFile::readFile(const std::string &file_path){
        // Don't know if the file is valid yet
        mIsValidFile = false;

        // Open up the file
        std::ifstream file(file_path.c_str(), std::ios::in | std::ios::binary);

        // Check if it could successfully open the file
        if(!file.good()){
            file.close();
            return ;
        }

        // Read the first four bytes (format) , it should say NES followed by 0x1A
        char format[4];
        file.read(format, 4);
        if(format[0] != 'N' || format[1] != 'E' || format[2] != 'S' || format[3] != 0x1A){
            file.close();
            return ;
        }

        // We know the file is a valid iNES file
        // Begin parsing the iNES file
        mIsValidFile = true;

        // Read the program rom and character rom
        char prgRomSize, chrRomSize;
        file.get(prgRomSize);
        file.get(chrRomSize);
        mHeader.prgRomSize = (uint8_t) prgRomSize;
        mHeader.chrRomSize = (uint8_t) chrRomSize;

        // Read the next 10 bytes, flags 6 and 7 are contained within the first 2 bytes of this array
        char flags[10];
        file.read(flags, 10);

        // Get the mirroring
        mHeader.mirroring = (flags[0] & 0x01) != 0? VERTICAL : HORIZONTAL;
        mHeader.mirroring = (flags[0] & 0x08) != 0? ALL : mHeader.mirroring;

        // Battery backing, and does it have a trainer?
        mHeader.batteryBacked = (flags[0] & 0x02) != 0;
        mHeader.hasTrainer = (flags[0] & 0x04) != 0;

        // Get the Mapper
        mHeader.mapperNumber = 0;
        mHeader.mapperNumber |= (flags[0] & 0xF0) >> 4;
        mHeader.mapperNumber |= (flags[1] & 0xF0);

        // Get PlayChoice 10 and VS Unisystem, we won't do much with these but we will be reading them
        mHeader.isVsUnisystem = (flags[1] & 0x01) != 0;
        mHeader.isPlayChoice10 = (flags[1] & 0x02) != 0;

        // If the cartridge has a trainer, read it
        if(mHeader.hasTrainer){

            // Allocate size for the trainer data
            mTrainerData = new uint8_t[512];

            // Read the next 512 bytes into the trainer data
            file.read((char *) mTrainerData, 512);
        }

        // Allocate size for the program and character rom
        mPrgRomData = new uint8_t[mHeader.prgRomSize * (2 << 13)];
        mChrRomData = new uint8_t[mHeader.chrRomSize * (2 << 13)];

        // Read program and character rom
        file.read((char *) mPrgRomData, mHeader.prgRomSize * (2 << 13));
        file.read((char *) mChrRomData, mHeader.chrRomSize * (2 << 13));

        // If the cartridge represents a PlayChoice 10 game, read 8 KB of data
        if(mHeader.isPlayChoice10){
            mPlayChoicePROMData = new uint8_t[2 << 12];
            // Read the next 8 KB into playchoice 10 ram
            file.read((char *) mPlayChoicePROMData, 2 << 12);
        }

        // Close the file
        file.close();
    }

    INESHeader &INESFile::getFileHeader(){
        return mHeader;
    }

    bool INESFile::isValid(){
        return mIsValidFile;
    }

    uint8_t *INESFile::getProgramRom(){
        return mPrgRomData;
    }

    uint8_t *INESFile::getCharacterRom(){
        return mChrRomData;
    }

}}