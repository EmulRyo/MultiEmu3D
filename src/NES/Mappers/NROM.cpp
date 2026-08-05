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

#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <iomanip>
#include <assert.h>
#include "NROM.h"

using namespace std;
using namespace Nes;

NROM::NROM(u8* buffer)
    :Mapper(buffer){

    m_chrBuffer = (m_chrBanks == 0) ? m_chrRam : m_chrData;
}

void NROM::Reset() {}

NametableMirroring NROM::GetNametableMirroring() const { return m_hardWireMirroring; }

u8 NROM::ReadPRG(u16 address) const {
    if (address < 0x8000)
        return 0x00;
    else if (address < 0xC000)
        return m_prgData[address - 0x8000];
    else {
        if (m_prgBanks == 2)
            return m_prgData[address - 0x8000];
        else
            return m_prgData[address - 0xC000];
    }
}

void NROM::WritePRG(u16 address, u8 value, u32 cpuCycle) {

}

u8 NROM::ReadCHR(u16 address) const {
	return m_chrBuffer[address];
}

void NROM::WriteCHR(u16 address, u8 value) {
    if (m_chrBanks == 0)
        m_chrBuffer[address] = value;
}

u8 NROM::GetMapperNum() const {
    return 0;
}

const char* NROM::GetMapperName() const {
    return "NROM";
}

u16 NROM::GetPRGBanks() const {
    return m_prgBanks;
}

u8 NROM::GetPRGBanksVisible() const {
    return 2;
}

u16 NROM::GetPRGBank(u8 number) const {
    if (number == 0)
        return 0;
    else
        return (m_prgBanks == 2) ? 1 : 0;
}

u16 NROM::GetCHRBanks() const {
    return 2;
}

u8 NROM::GetCHRBanksVisible() const {
    return 2;
}

u16 NROM::GetCHRBank(u8 number) const {
    return number;
}

void NROM::SaveState(std::ostream* stream) {}
void NROM::LoadState(std::istream* stream) {}

void NROM::Extract() {}
