/*
 This file is part of MultiEmu3D.
 
 MultiEmu3D is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MultiEmu3D is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MultiEmu3D.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __NES_CARTRIDGE_H__
#define __NES_CARTRIDGE_H__

#include <string>
#include "../Common/Types.h"

namespace Nes {
    
    enum class NametableMirroring { HORIZONTAL, VERTICAL, SINGLE_SCREEN, FOUR_SCREEN };

    class Mapper;

    class Cartridge
    {   
    public:
        Cartridge(std::string fileName, std::string batteriesPath="", u8 *cartridgeBuffer=NULL, unsigned long size=0);
        ~Cartridge();
        
        void Reset();
        
        u8 *GetData();
        unsigned int GetSize();
        std::string GetName();
        bool IsLoaded();

        NametableMirroring GetNametableMirroring();

        u8   ReadPRG(u16 address);
        void WritePRG(u16 address, u8 value);
        u8   ReadCHR(u16 address);
        void WriteCHR(u16 address, u8 value);
        
        void SaveState(std::ostream *stream);
        void LoadState(std::istream *stream);
        
        void Extract();

        u8 GetMapperNum();
        const char* GetMapperName();

        u8 GetPRGBanks();
        u8 GetPRGBank0();
        u8 GetPRGBank1();

        u8 GetCHRBanks();
        u8 GetCHRBank0();
        u8 GetCHRBank1();

    private:
        unsigned long m_romSize;
        std::string m_name;
        bool m_isLoaded;
        u8*  m_buffer;
        NametableMirroring m_nametableMirroring;
        Mapper* m_mapper;

        void LoadFile(std::string fileName, std::string batteriesPath);
        void ReadHeader();
        std::string GetShortName(std::string fileName);
    };
}

#endif
