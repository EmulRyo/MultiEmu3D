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
}

NametableMirroring MMC1::GetNametableMirroring() {
    if (m_nametableMirroring == NametableMirroring::HORIZONTAL) {
        u8 mirrorID = m_regs[REG_CONTROL] & 0x03;
        if (mirrorID < 2)
            return NametableMirroring::SINGLE_SCREEN;
        else if (mirrorID == 2)
            return NametableMirroring::VERTICAL;
        else
            return NametableMirroring::HORIZONTAL;
    }
    else
        return m_nametableMirroring;
}

u8 MMC1::ReadPRG(u16 address) {
    if (address < 0x8000) {
        return m_prgRam[address - 0x6000];
    }
    else {
        u8 mode = (m_regs[REG_CONTROL] >> 2) & 0x03;
        u8 bank = m_regs[REG_PRGBANK] & 0x0F;
        if (mode < 2) { // Modo 32 KB
            bank = bank & 0x0E;
            return m_prgData[(bank * 0x8000) + address - 0x8000];
        }
        else { // Modo 16 KB
            if (address < 0xC000) {
                if (mode == 2)
                    bank = 0;

                return m_prgData[(bank * 0x4000) + address - 0x8000];
            }
            else {
                if (mode == 3)
                    bank = m_prgBanks - 1;

                return m_prgData[(bank * 0x4000) + address - 0xC000];
            }
        }
    }
}

void MMC1::WritePRG(u16 address, u8 value) {
    if ((address >= 0x8000) && (address <= 0xFFFF)) {
        m_numWrites = (m_numWrites + 1) % 5;
        if (BIT7(value) == 0) {
            m_shiftRegister = m_shiftRegister >> 1;
            m_shiftRegister = (BIT0(value) << 4) | m_shiftRegister;
            if (m_numWrites == 0) {
                u8 regId = (address >> 13) & 0x3;
                m_regs[regId] = m_shiftRegister;
                m_shiftRegister = 0;
            }
        }
        else {
            m_shiftRegister = 0;
            m_numWrites = 0;
            m_regs[REG_CONTROL] |= 0x0C;
        }
    }
}

u8 MMC1::ReadCHR(u16 address) {
    u8 mode = m_regs[REG_CONTROL] >> 4;
    u8 bank = m_regs[REG_CHRBANK0] & 0x1F;
    u8* buffer = (m_chrBanks == 0) ? m_chrRam : m_chrData;
    if (mode == 0) {// 8KB mode
        bank &= 0x1E;
        return buffer[(bank * 0x2000) + address];
    }
    else { // 4KB mode
        if (address < 0x1000)
            return buffer[(bank * 0x1000) + address];
        else {
            u8 bank = m_regs[REG_CHRBANK1] & 0x1F;
            return buffer[(bank * 0x1000) + (address-0x1000)];
        }
    }
}

void MMC1::WriteCHR(u16 address, u8 value) {

    u8 mode = m_regs[REG_CONTROL] >> 4;
    u8 bank = m_regs[REG_CHRBANK0] & 0x1F;
    u8* buffer = m_chrRam;
    if (mode == 0) {// 8KB mode
        bank &= 0x1E;
        buffer[(bank * 0x2000) + address] = value;
    }
    else { // 4KB mode
        if (address < 0x1000)
            buffer[(bank * 0x1000) + address] = value;
        else {
            u8 bank = m_regs[REG_CHRBANK1] & 0x1F;
            buffer[(bank * 0x1000) + (address-0x1000)] = value;
        }
    }
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
    u8 mode = (m_regs[REG_CONTROL] >> 2) & 0x03;
    if (mode < 2) // Modo 32 KB
        return m_regs[REG_PRGBANK] & 0x0E;
    else // Modo 16 KB
        return (mode == 2) ? 0 : m_regs[REG_PRGBANK] & 0x0F;
}

u8 MMC1::GetPRGBank1() {
    u8 mode = (m_regs[REG_CONTROL] >> 2) & 0x03;
    if (mode < 2) // Modo 32 KB
        return (m_regs[REG_PRGBANK] & 0x0E) + 1;
    else // Modo 16 KB
        return (mode == 3) ? m_prgBanks - 1 : m_regs[REG_PRGBANK] & 0x0F;
}

u8 MMC1::GetCHRBanks() {
    return m_chrBanks;
}

u8 MMC1::GetCHRBank0() {
    u8 mode = m_regs[REG_CONTROL] >> 4;
    if (mode == 0)// 8KB mode
        return m_regs[REG_CHRBANK0] & 0x1E;
    else // 4KB mode
        return m_regs[REG_CHRBANK0] & 0x1F;
}

u8 MMC1::GetCHRBank1() {
    u8 mode = m_regs[REG_CONTROL] >> 4;
    if (mode == 0) // 8KB mode
        return (m_regs[REG_CHRBANK0] & 0x1E) + 1;
    else // 4KB mode
        return (m_regs[REG_CHRBANK1] & 0x1F);
}

void MMC1::SaveState(std::ostream* stream) {}
void MMC1::LoadState(std::istream* stream) {}

void MMC1::Extract() {}
