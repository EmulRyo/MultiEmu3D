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

NametableMirroring CNROM::GetNametableMirroring() {
    return m_hardWireMirroring;
}

u8 CNROM::ReadPRG(u16 address) {
    if (address < 0xC000)
        return m_prgData[(GetPRGBank0() * 0x4000) + address - 0x8000];
    else
        return m_prgData[(GetPRGBank1() * 0x4000) + address - 0xC000];
}

void CNROM::WritePRG(u16 address, u8 value) {
    if ((address >= 0x8000) && (address <= 0xFFFF)) {
        m_chrBank = value & (m_chrBanks-1);
    }
}

u8 CNROM::ReadCHR(u16 address) {
    return m_chrData[(GetCHRBank0() * 0x2000) + address];
}

void CNROM::WriteCHR(u16 address, u8 value) {
    //assert(false && "Trying to write to CHR");
}

u8 CNROM::GetMapperNum() {
    return 3;
}

const char* CNROM::GetMapperName() {
    return "CNROM";
}

u8 CNROM::GetPRGBanks() {
    return m_prgBanks;
}

u8 CNROM::GetPRGBank0() {
    return 0;
}

u8 CNROM::GetPRGBank1() {
    return (m_prgBanks > 0) ? 1 : 0;
}

u8 CNROM::GetCHRBanks() {
    return m_chrBanks;
}

u8 CNROM::GetCHRBank0() {
    return m_chrBank;
}

u8 CNROM::GetCHRBank1() {
    return m_chrBank;
}

void CNROM::SaveState(std::ostream* stream) {
    stream->write((char*)&m_chrBank, sizeof(u8));
}

void CNROM::LoadState(std::istream* stream) {
    stream->read((char*)&m_chrBank, sizeof(u8));
}

void CNROM::Extract() {}
