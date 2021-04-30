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
#include "MMC1.h"

using namespace std;
using namespace Nes;

#define REG_CONTROL  0
#define REG_CHRBANK0 1
#define REG_CHRBANK1 2
#define REG_PRGBANK  3

MMC1::MMC1(u8* buffer)
    :Mapper(buffer){

    Reset();
}

void MMC1::Reset() {
    m_shiftRegister = 0;
    m_numWrites = 0;
    m_regs[REG_CONTROL]  = 0x0C;
    m_regs[REG_CHRBANK0] = 0x00;
    m_regs[REG_CHRBANK1] = 0x00;
    m_regs[REG_PRGBANK]  = 0x00;
    m_chrBuffer = (m_chrBanks == 0) ? m_chrRam : m_chrData;
    UpdateMirroring();
    UpdatePRGBanks();
    UpdateCHRBanks();
}

NametableMirroring MMC1::GetNametableMirroring() {
    return m_mapperMirroring;
}

u8 MMC1::ReadPRG(u16 address) {
    if (address < 0x8000)
        return m_prgRam[address - 0x6000];
    else if (address < 0xC000)
        return m_prgData[(GetPRGBank0() * 0x4000) + address - 0x8000];
    else
        return m_prgData[(GetPRGBank1() * 0x4000) + address - 0xC000];
}

void MMC1::WritePRG(u16 address, u8 value) {
    if (address < 0x8000) {
        m_prgRam[address - 0x6000] = value;
    }
    else if ((address >= 0x8000) && (address <= 0xFFFF)) {
        m_numWrites = (m_numWrites + 1) % 5;
        if (BIT7(value) == 0) {
            m_shiftRegister = m_shiftRegister >> 1;
            m_shiftRegister = (BIT0(value) << 4) | m_shiftRegister;
            if (m_numWrites == 0) {
                u8 regId = (address >> 13) & 0x3;
                m_regs[regId] = m_shiftRegister;
                m_shiftRegister = 0;

                UpdateMirroring();
                UpdatePRGBanks();
                UpdateCHRBanks();
            }
        }
        else {
            m_shiftRegister = 0;
            m_numWrites = 0;
            m_regs[REG_CONTROL] |= 0x0C;

            UpdateMirroring();
            UpdatePRGBanks();
            UpdateCHRBanks();
        }
    }
}

u8 MMC1::ReadCHR(u16 address) {
    if (address < 0x1000)
        return m_chrBuffer[(GetCHRBank0() * 0x1000) + address];
    else
        return m_chrBuffer[(GetCHRBank1() * 0x1000) + (address - 0x1000)];
}

void MMC1::WriteCHR(u16 address, u8 value) {

    if (address < 0x1000)
        m_chrBuffer[(GetCHRBank0() * 0x1000) + address] = value;
    else
        m_chrBuffer[(GetCHRBank1() * 0x1000) + (address - 0x1000)] = value;
}

u8 MMC1::GetMapperNum() {
    return 1;
}

const char* MMC1::GetMapperName() {
    return "MMC1";
}

u8 MMC1::GetPRGBanks() {
    return m_prgBanks;
}

u8 MMC1::GetPRGBank0() {
    return m_prgBank0;
}

u8 MMC1::GetPRGBank1() {
    return m_prgBank1;
}

u8 MMC1::GetCHRBanks() {
    return m_chrBanks;
}

u8 MMC1::GetCHRBank0() {
    return m_chrBank0;
}

u8 MMC1::GetCHRBank1() {
    return m_chrBank1;
}

void MMC1::SaveState(std::ostream* stream) {}
void MMC1::LoadState(std::istream* stream) {}

void MMC1::Extract() {}

void MMC1::UpdateMirroring() {
    u8 mirrorID = m_regs[REG_CONTROL] & 0x03;
    if (mirrorID < 2)
        m_mapperMirroring = NametableMirroring::SINGLE_SCREEN;
    else if (mirrorID == 2)
        m_mapperMirroring = NametableMirroring::VERTICAL;
    else
        m_mapperMirroring = NametableMirroring::HORIZONTAL;
}

void MMC1::UpdatePRGBanks() {
    u8 mode = (m_regs[REG_CONTROL] >> 2) & 0x03;
    u8 bank = m_regs[REG_PRGBANK] & 0x0F;
    if (mode < 2) { // Modo 32 KB
        m_prgBank0 = bank & 0x0E;
        m_prgBank1 = m_prgBank0 + 1;
    }
    else if (mode == 2) {
        m_prgBank0 = 0;
        m_prgBank1 = bank;
    }
    else {
        m_prgBank0 = bank;
        m_prgBank1 = m_prgBanks - 1;
    }
}

void MMC1::UpdateCHRBanks() {
    u8 mode = m_regs[REG_CONTROL] >> 4;
    m_chrBank0 = m_regs[REG_CHRBANK0] & 0x1F;
    m_chrBank1 = m_regs[REG_CHRBANK1] & 0x1F;
    u8* buffer = m_chrRam;
    if (mode == 0) { // 8KB mode
        m_chrBank0 &= 0x1E;
        m_chrBank1 = m_chrBank0+1;
    }
}
