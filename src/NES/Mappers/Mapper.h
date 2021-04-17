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

#ifndef __NES_MAPPER_H__
#define __NES_MAPPER_H__

#include <string>
#include "../../Common/Types.h"
#include "../Cartridge.h"

namespace Nes {
    
    class Mapper
    {   
    public:
        Mapper(u8* buffer);
        virtual void Reset() = 0;

        virtual NametableMirroring GetNametableMirroring() = 0;

        virtual u8   ReadPRG(u16 address)            = 0;
        virtual void WritePRG(u16 address, u8 value) = 0;
        virtual u8   ReadCHR(u16 address)            = 0;
        virtual void WriteCHR(u16 address, u8 value) = 0;
        
        virtual void SaveState(std::ostream *stream) = 0;
        virtual void LoadState(std::istream *stream) = 0;
        
        virtual void Extract() = 0;

        virtual u8 GetMapperNum() = 0;
        virtual const char *GetMapperName() = 0;

        virtual u8 GetPRGBanks() = 0;
        virtual u8 GetPRGBank0() = 0;
        virtual u8 GetPRGBank1() = 0;

        virtual u8 GetCHRBanks() = 0;
        virtual u8 GetCHRBank0() = 0;
        virtual u8 GetCHRBank1() = 0;

        static Mapper* Create(u16 mapperId, u8* buffer);

    protected:
        u8*  m_prgData;
        u8*  m_chrData;
        u8   m_prgBanks;
        u8   m_chrBanks;
        NametableMirroring m_nametableMirroring;
    };
}

#endif
