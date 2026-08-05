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

        NametableMirroring GetNametableMirroring() const;

        u8   ReadPRG(u16 address) const;
        void WritePRG(u16 address, u8 value, u32 cpuCycle);
        u8   ReadCHR(u16 address) const;
        void WriteCHR(u16 address, u8 value);
        
        void SaveState(std::ostream *stream);
        void LoadState(std::istream *stream);
        
        void Extract();

        u8 GetMapperNum() const;
        const char* GetMapperName() const;

        u16 GetPRGBanks() const;
        u8  GetPRGBanksVisible() const;
        u16 GetPRGBank(u8 number) const;

        u16 GetCHRBanks() const;
        u8 GetCHRBanksVisible() const;
        u16 GetCHRBank(u8 number) const;

    private:
        u8 m_shiftRegister;
        u8 m_numWrites;
        u8 m_regs[4];
        u8 m_prgRam[0x2000];
        u8 m_chrRam[0x2000];

        u8* m_chrBuffer;
        u8 m_prgBank0;
        u8 m_prgBank1;
        u8 m_chrBank0;
        u8 m_chrBank1;
        u32 m_lastRegisterWriteCycle;

        void UpdateMirroring();
        void UpdatePRGBanks();
        void UpdateCHRBanks();
    };
}

#endif
