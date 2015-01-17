/*
 This file is part of DMGBoy.
 
 DMGBoy is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 DMGBoy is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with DMGBoy.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __CARTRIDGE_H__
#define __CARTRIDGE_H__

#include <string>
#include "../Common/Types.h"

namespace MasterSystem {
    
    class Cartridge
    {
        struct Ram
        {
            u8   mem[0x8000];
            bool enabled;
            u8   numBank;
            u8  *page;
        };
        
    private:
        unsigned long m_romSize;
        std::string m_name;
        bool m_isLoaded;
        u8  *m_buffer;
        u8  *m_mem;
        u8  *m_pages[3];
        u8   m_numBanks[3];
        u8   m_maskPages;
        Ram  m_ram;

        void LoadFile(std::string fileName, std::string batteriesPath);
        std::string GetShortName(std::string fileName);
        
    public:
        Cartridge(std::string fileName, std::string batteriesPath="", u8 *cartridgeBuffer=NULL, unsigned long size=0);
        ~Cartridge();
        
        void Reset();
        
        u8 *GetData();
        unsigned int GetSize();
        std::string GetName();
        bool IsLoaded();

        u8 Read(u16 address);
        void Write(u16 address, u8 value);
        
        void SaveStateMBC(std::ostream *stream);
        void LoadStateMBC(std::istream *stream);
        
        void Extract();
        
        u8   GetNumBank(u8 bank);
        bool GetRAMEnabled();
        u8   GetRAMNumBank();
    };
}

#endif
