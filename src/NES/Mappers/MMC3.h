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

#pragma once

#include <string>
#include "../../Common/Types.h"
#include "Mapper.h"

namespace Nes {
    
    class MMC3: public Mapper
    {   
    public:
        
        MMC3(u8* buffer);
        ~MMC3();
        
        void Reset();

        NametableMirroring GetNametableMirroring() const;

        u8   ReadPRG(u16 address);
        void WritePRG(u16 address, u8 value);
        u8   ReadCHR(u16 address) const;
        void WriteCHR(u16 address, u8 value);

        void Scanline() override;
        bool IRQ() override;
        
        void SaveState(std::ostream *stream);
        void LoadState(std::istream *stream);
        
        void Extract();

        u8 GetMapperNum();
        const char* GetMapperName();

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
        struct IRQ_t {
            u8      counter;
            bool    enabled;
            bool    reload;
            bool    triggered;
        };

        u8 m_regs[5];
        u8 m_prgRam[0x2000];
        u8 m_chrRam[0x2000];

        u8* m_chrBuffer;
        u8 m_prgBank[4];
        u8 m_chrBank[8];

        IRQ_t m_IRQ;

        void OnBankSelect(u8 value);
        void OnBankData(u8 value);
        void OnMirroring(u8 value);
        void OnPRGRAMProtect(u8 value);
        void OnIRQLatch(u8 value);
        void OnIRQReload(u8 value);
        void OnIRQDisable(u8 value);
        void OnIRQEnable(u8 value);
    };
}
