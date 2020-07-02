#pragma once

#include <stdint.h>
#include <string>

namespace VNES{ namespace FileParser {

/**
 * @brief The different mirrorings for nametables
 * 
 * VERTICAL - Vertical Mirroring
 * HORIZONTAL - Horizontal Mirroring
 * ALL - 4 screen variation
 */
enum NametableMirroring {
    VERTICAL,
    HORIZONTAL,
    ALL
};

/**
 * @brief Header for an iNES file
 */
struct INESHeader {

    // Rom size
    uint8_t prgRomSize; // Size of program rom in 16 KB units
    uint8_t chrRomSize; // Size of the character rom in 8 KB units. 0 means CHR RAM will be used

    // Mapper information 
    uint8_t mapperNumber; // Which mapper number is this
    bool ignoreMirroring; // Is mirroring ignored

    NametableMirroring mirroring; // What type of mirroring is used by this mapper
    bool batteryBacked; // Does the game have battery backed PRG RAM
    bool hasTrainer; // Does a trainer exist
    bool isVsUnisystem; // Is this a vs unisystem
    bool isPlayChoice10; // playchoice10

    // TV information
    bool isNTSC; // Is this an NTSC compatible rom

};

/**
 * @brief INESFile Data Structure
 * 
 * An instance represents an iNES 1.0 file.
 * 
 */
class INESFile {
    public:
        INESFile();
        ~INESFile();

        /**
         * @brief Reads and parses an iNES file
         * 
         * @param file_path 
         */
        void readFile(const std::string &file_path);

        /**
         * @brief Get the iNES File Header
         * 
         * @return INESHeader reference
         */
        INESHeader &getFileHeader();

        /**
         * @brief Checks if the object represents a valid iNES file
         * 
         * @return true - read a valid iNES file
         * @return false - either file read was not valid or no file was ever read
         */
        bool isValid();

        /**
         * @brief Get the program rom data
         * 
         * @return uint8_t - pointer to array of data containing the program rom data
         */
        uint8_t *getProgramRom();

        /**
         * @brief Get the character rom data
         * 
         * @return uint8_t - pointer to array of data containing the character data
         */
        uint8_t *getCharacterRom();

    private:

    INESHeader mHeader;

    // Program and character rom data
    uint8_t *mPrgRomData;
    uint8_t *mChrRomData;

    // Trainer data 
    uint8_t *mTrainerData;
    
    // Play choice data
    uint8_t *mPlayChoiceINSTData;
    uint8_t *mPlayChoicePROMData;

    // Is this a valid file
    bool mIsValidFile;
};

}}