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
    
    class Cartridge
    {
        struct Ram
        {
            u8   mem[0x8000];
            bool enabled;
            u8   numBank;
            u8  *page;
        };
        
    public:
        Cartridge(std::string fileName, std::string batteriesPath="", u8 *cartridgeBuffer=NULL, unsigned long size=0);
        ~Cartridge();
        
        void Reset();
        
        u8 *GetData();
        unsigned int GetSize();
        std::string GetName();
        bool IsLoaded();

        u8   Read(u16 address);
        void Write(u16 address, u8 value);
        
        void SaveStateMBC(std::ostream *stream);
        void LoadStateMBC(std::istream *stream);
        
        void Extract();

    private:
        unsigned long m_romSize;
        std::string m_name;
        bool m_isLoaded;
        u8*  m_buffer;
        u8*  m_prgData;
        u8*  m_chrData;
        u8   m_prgBanks;
        u8   m_chrBanks;

        void LoadFile(std::string fileName, std::string batteriesPath);
        void ReadHeader();
        std::string GetShortName(std::string fileName);
        u32 RoundUpPowerOf2(u32 v);
    };
}

#endif
