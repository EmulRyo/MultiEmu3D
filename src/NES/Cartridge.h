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
        
        unsigned int GetSize() const;
        std::string GetName() const;
        bool IsLoaded() const;

        NametableMirroring GetNametableMirroring() const;

        u8   ReadPRG(u16 address) const;
        void WritePRG(u16 address, u8 value);
        u8   ReadCHR(u16 address) const;
        void WriteCHR(u16 address, u8 value);

        void Scanline();
        bool IRQ();
        
        void SaveState(std::ostream *stream);
        void LoadState(std::istream *stream);
        
        void Extract();

        u8 GetMapperNum() const;
        const char* GetMapperName() const;

        u8 GetPRGBanks() const;
        u8 GetPRGBanksVisible()  const;
        u8 GetPRGBank(u8 number) const;

        u8 GetCHRBanks() const;
        u8 GetCHRBanksVisible() const;
        u8 GetCHRBank(u8 number) const;

        bool HasIRQ() const;
        u8   GetIRQReloadValue() const;
        u8   GetIRQCounter() const;
        bool GetIRQReloadFlag() const;
        bool GetIRQEnabled() const;

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
