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

#ifndef __NES_NROM_H__
#define __NES_NROM_H__

#include <string>
#include "../../Common/Types.h"
#include "Mapper.h"

namespace Nes {
    
    class NROM: public Mapper
    {   
    public:
        
        NROM(u8* buffer);
        ~NROM();
        
        void Reset();

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
        u8 m_chrRam[0x2000];
        u8* m_chrBuffer;
    };
}

#endif
