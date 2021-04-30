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

#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <iomanip>
#include <assert.h>
#include "..\..\Common\Bit.h"
#include "MMC3.h"

using namespace std;
using namespace Nes;

#define REG_BANKSELECT  0
#define REG_BANKDATA    1
#define REG_MIRRORING   2
#define REG_PRGRAM      3
#define REG_IRQLATCH    4
#define REG_IRQRELOAD   5
#define REG_IRQDISABLE  6
#define REG_IRQENABLE   7

MMC3::MMC3(u8* buffer)
    :Mapper(buffer){
    // El numero de bancos de PRG en la cabecera del cartucho esta especificado en 16KB.
    // Lo multiplicamos por 2 para tener el numero de bancos en 8KB
    m_prgBanks *= 2;
    // El numero de bancos de CHRen la cabecera del cartucho esta especificado en 8KB.
    // Lo multiplicamos por 8 para tener el numero de bancos en 1KB
    m_chrBanks *= 8;

    Reset();
}

void MMC3::Reset() {
    
    m_chrBuffer = (m_chrBanks == 0) ? m_chrRam : m_chrData;

    m_regs[REG_BANKSELECT] = 0;
    m_regs[REG_BANKDATA] = 0;
    m_regs[REG_MIRRORING] = 0;

    m_prgBank[0] = 0;
    m_prgBank[1] = 1;
    m_prgBank[2] = m_prgBanks - 2;
    m_prgBank[3] = m_prgBanks - 1;
    for (int i = 0; i < 8; i++)
        m_chrBank[i] = i;

    if (m_hardWireMirroring == NametableMirroring::FOUR_SCREEN)
        m_mapperMirroring = NametableMirroring::FOUR_SCREEN;
    else
        m_mapperMirroring = NametableMirroring::HORIZONTAL;
}

NametableMirroring MMC3::GetNametableMirroring() {
    return m_mapperMirroring;
}

u8 MMC3::ReadPRG(u16 address) {
    if (address < 0x8000)
        return m_prgRam[address - 0x6000];
    else {
        u8 addressZone = (address - 0x8000) / 0x2000;
        return m_prgData[(m_prgBank[addressZone] * 0x2000) + (address - 0x8000 - (0x2000 * addressZone))];
    }
}

void MMC3::WritePRG(u16 address, u8 value) {
    if (address < 0x8000) {
        m_prgRam[address - 0x6000] = value;
    }
    else if ((address >= 0x8000) && (address < 0xA000)) {
        if ((address & 0001) == 0)
            OnBankSelect(value);
        else
            OnBankData(value);
    }
    else if ((address >= 0xA000) && (address < 0xC000)) {
        if ((address & 0001) == 0)
            OnMirroring(value);
        else
            m_regs[REG_PRGRAM] = value;
    }
}

u8 MMC3::ReadCHR(u16 address) {
    u8 addressZone = address / 0x400;
    return m_chrBuffer[(m_chrBank[addressZone] * 0x400) + (address - (0x400*addressZone))];
}

void MMC3::WriteCHR(u16 address, u8 value) {
    u8 addressZone = address / 0x400;
    m_chrBuffer[(m_chrBank[addressZone] * 0x400) + (address - (0x400 * addressZone))] = value;
}

u8 MMC3::GetMapperNum() {
    return 4;
}

const char* MMC3::GetMapperName() {
    return "MMC3";
}

u8 MMC3::GetPRGBanks() {
    return m_prgBanks;
}

u8 MMC3::GetPRGBank0() {
    return m_prgBank[0];
}

u8 MMC3::GetPRGBank1() {
    return m_prgBank[1];
}

u8 MMC3::GetCHRBanks() {
    return m_chrBanks;
}

u8 MMC3::GetCHRBank0() {
    return m_chrBank[0];
}

u8 MMC3::GetCHRBank1() {
    return m_chrBank[1];
}

void MMC3::SaveState(std::ostream* stream) {}
void MMC3::LoadState(std::istream* stream) {}

void MMC3::Extract() {}

void MMC3::OnBankSelect(u8 value) {
    u8 oldPRGMode = BIT6(m_regs[REG_BANKSELECT]);
    u8 newPRGMode = BIT6(value);

    if (oldPRGMode != newPRGMode) {
        if (newPRGMode) {
            u8 r6 = m_prgBank[0];
            u8 r7 = m_prgBank[1];
            m_prgBank[0] = m_prgBanks - 2;
            m_prgBank[1] = r7;
            m_prgBank[2] = r6;
            m_prgBank[3] = m_prgBanks - 1;
        }
        else {
            u8 r6 = m_prgBank[2];
            u8 r7 = m_prgBank[1];
            m_prgBank[0] = r6;
            m_prgBank[1] = r7;
            m_prgBank[2] = m_prgBanks - 2;
            m_prgBank[3] = m_prgBanks - 1;
        }
    }

    u8 oldCHRMode = BIT7(m_regs[REG_BANKSELECT]);
    u8 newCHRMode = BIT7(value);

    if (oldCHRMode != newCHRMode) {
        if (newCHRMode) {
            u8 r0 = m_chrBank[0];
            u8 r1 = m_chrBank[2];
            u8 r2 = m_chrBank[4];
            u8 r3 = m_chrBank[5];
            u8 r4 = m_chrBank[6];
            u8 r5 = m_chrBank[7];
            m_chrBank[0] = r2;
            m_chrBank[1] = r3;
            m_chrBank[2] = r4;
            m_chrBank[3] = r5;
            m_chrBank[4] = r0;
            m_chrBank[5] = r0+1;
            m_chrBank[6] = r1;
            m_chrBank[7] = r1+1;
        }
        else {
            u8 r0 = m_chrBank[4];
            u8 r1 = m_chrBank[6];
            u8 r2 = m_chrBank[0];
            u8 r3 = m_chrBank[1];
            u8 r4 = m_chrBank[2];
            u8 r5 = m_chrBank[3];
            m_chrBank[0] = r0;
            m_chrBank[1] = r0+1;
            m_chrBank[2] = r1;
            m_chrBank[3] = r1+1;
            m_chrBank[4] = r2;
            m_chrBank[5] = r3;
            m_chrBank[6] = r4;
            m_chrBank[7] = r5;
        }
    }

    m_regs[REG_BANKSELECT] = value;
}

void MMC3::OnBankData(u8 value) {
    u8 bankRegister = m_regs[REG_BANKSELECT] & 0x07;
    if (bankRegister > 5) { // PRG Banks
        if (bankRegister == 7) // r7
            m_prgBank[1] = value;
        else { // r6
            u8 PRGMode = BIT6(m_regs[REG_BANKSELECT]);
            if (PRGMode)
                m_prgBank[2] = value;
            else
                m_prgBank[0] = value;
        }
    }
    else { // CHR Banks
        u8 CHRMode = BIT7(m_regs[REG_BANKSELECT]);
        if (bankRegister < 2)
            value &= 0xFE;

        if (CHRMode == 1) {
            if (bankRegister == 0) {
                m_chrBank[4] = value;
                m_chrBank[5] = value + 1;
            }
            else if (bankRegister == 1) {
                m_chrBank[6] = value;
                m_chrBank[7] = value + 1;
            }
            else
                m_chrBank[bankRegister - 2] = value;
        }
        else {
            if (bankRegister == 0) {
                m_chrBank[0] = value;
                m_chrBank[1] = value+1;
            }
            else if (bankRegister == 1) {
                m_chrBank[2] = value;
                m_chrBank[3] = value + 1;
            }
            else
                m_chrBank[bankRegister + 2] = value;
        }
    }

    m_regs[REG_BANKDATA] = value;
}

void MMC3::OnMirroring(u8 value) {
    if (m_hardWireMirroring == NametableMirroring::FOUR_SCREEN)
        m_mapperMirroring = NametableMirroring::FOUR_SCREEN;
    else
        m_mapperMirroring = BIT0(value) ? NametableMirroring::HORIZONTAL : NametableMirroring::VERTICAL;

    m_regs[REG_MIRRORING] = value;
}
