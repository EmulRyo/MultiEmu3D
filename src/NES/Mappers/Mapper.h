/*
 This file is part of MARS.
 
 MARS is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MARS is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MARS.  If not, see <http://www.gnu.org/licenses/>.
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

        virtual NametableMirroring GetNametableMirroring() const = 0;

        virtual u8   ReadPRG(u16 address) const      = 0;
        virtual void WritePRG(u16 address, u8 value, u32 cpuCycle) = 0;
        virtual u8   ReadCHR(u16 address) const      = 0;
        virtual void WriteCHR(u16 address, u8 value) = 0;

        virtual void Scanline() {};
        virtual bool IRQ() { return false; };
        
        virtual void SaveState(std::ostream *stream) = 0;
        virtual void LoadState(std::istream *stream) = 0;
        
        virtual void Extract() = 0;

        virtual u8 GetMapperNum() const = 0;
        virtual const char *GetMapperName() const = 0;

        virtual u16 GetPRGBanks() const = 0;
        virtual u8  GetPRGBanksVisible() const = 0;
        virtual u16 GetPRGBank(u8 number) const = 0;

        virtual u16 GetCHRBanks() const = 0;
        virtual u8  GetCHRBanksVisible() const = 0;
        virtual u16 GetCHRBank(u8 number) const = 0;

        virtual bool HasIRQ() const { return false; };
        virtual u8   GetIRQReloadValue() const { return 0; };
        virtual u8   GetIRQCounter() const { return 0; };
        virtual bool GetIRQReloadFlag() const { return false; };
        virtual bool GetIRQEnabled() const { return false; };

        static Mapper* Create(u16 mapperId, u8* buffer);

    protected:
        u8*  m_prgData;
        u8*  m_chrData;
        u16  m_prgBanks;
        u16  m_chrBanks;
        NametableMirroring m_hardWireMirroring;
        NametableMirroring m_mapperMirroring;
    };
}

#endif
