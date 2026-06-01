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
#include "CNROM.h"

using namespace std;
using namespace Nes;

CNROM::CNROM(u8* buffer)
    :Mapper(buffer){

    Reset();
}

void CNROM::Reset() {
    assert((m_chrBanks > 0) && "No CHR rom");
    m_mapperMirroring = m_hardWireMirroring;
}

NametableMirroring CNROM::GetNametableMirroring() const {
    return m_hardWireMirroring;
}

u8 CNROM::ReadPRG(u16 address) const {
    if (address < 0x8000)
        return 0x00;
    else if (address < 0xC000)
        return m_prgData[(GetPRGBank(0) * 0x4000) + address - 0x8000];
    else
        return m_prgData[(GetPRGBank(1) * 0x4000) + address - 0xC000];
}

void CNROM::WritePRG(u16 address, u8 value, u32 cpuCycle) {
    if ((address >= 0x8000) && (address <= 0xFFFF)) {
        m_chrBank = value & (m_chrBanks-1);
    }
}

u8 CNROM::ReadCHR(u16 address) const {
    return m_chrData[(GetCHRBank(0) * 0x2000) + address];
}

void CNROM::WriteCHR(u16 address, u8 value) {
    //assert(false && "Trying to write to CHR");
}

u8 CNROM::GetMapperNum() const {
    return 3;
}

const char* CNROM::GetMapperName() const {
    return "CNROM";
}

u16 CNROM::GetPRGBanks() const {
    return m_prgBanks;
}

u8 CNROM::GetPRGBanksVisible() const {
    return 2;
}

u16 CNROM::GetPRGBank(u8 number) const {
    if (number == 0)
        return 0;
    else
        return (m_prgBanks > 0) ? 1 : 0;
}

u16 CNROM::GetCHRBanks() const {
    return m_chrBanks;
}

u8 CNROM::GetCHRBanksVisible() const {
    return 1;
}

u16 CNROM::GetCHRBank(u8 number) const {
    return m_chrBank;
}

void CNROM::SaveState(std::ostream* stream) {
    stream->write((char*)&m_chrBank, sizeof(u8));
}

void CNROM::LoadState(std::istream* stream) {
    stream->read((char*)&m_chrBank, sizeof(u8));
}

void CNROM::Extract() {}
