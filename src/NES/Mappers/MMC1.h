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

#ifndef __NES_MMC1_H__
#define __NES_MMC1_H__

#include <string>
#include "../../Common/Types.h"
#include "Mapper.h"

namespace Nes {
    
    class MMC1: public Mapper
    {   
    public:
        
        MMC1(u8* buffer);
        ~MMC1();
        
        void Reset();

        NametableMirroring GetNametableMirroring();

        u8   ReadPRG(u16 address);
        void WritePRG(u16 address, u8 value);
        u8   ReadCHR(u16 address);
        void WriteCHR(u16 address, u8 value);
        
        void SaveState(std::ostream *stream);
        void LoadState(std::istream *stream);
        
        void Extract();

    private:
        u8 m_shiftRegister;
        u8 m_numWrites;
        u8 m_regs[4];
        u8 m_prgRam[0x2000];
        u8 m_chrRam[0x2000];
    };
}

#endif
