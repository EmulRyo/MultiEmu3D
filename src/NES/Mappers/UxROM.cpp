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
#include "UxROM.h"

using namespace std;
using namespace Nes;

UxROM::UxROM(u8* buffer)
    :Mapper(buffer){

    Reset();
}

void UxROM::Reset() {
    m_prgBank = 0;
    m_mapperMirroring = m_hardWireMirroring;
}

NametableMirroring UxROM::GetNametableMirroring() {
    return m_hardWireMirroring;
}

u8 UxROM::ReadPRG(u16 address) {
    if (address < 0xC000)
        return m_prgData[(GetPRGBank0() * 0x4000) + address - 0x8000];
    else
        return m_prgData[(GetPRGBank1() * 0x4000) + address - 0xC000];
}

void UxROM::WritePRG(u16 address, u8 value) {
    if ((address >= 0x8000) && (address <= 0xFFFF)) {
        m_prgBank = value & 0x0F;
    }
}

u8 UxROM::ReadCHR(u16 address) {
    u8* buffer = (m_chrBanks == 0) ? m_chrRam : m_chrData;
    return buffer[address];
}

void UxROM::WriteCHR(u16 address, u8 value) {
    m_chrRam[address] = value;
}

u8 UxROM::GetMapperNum() {
    return 2;
}

const char* UxROM::GetMapperName() {
    return "UxROM";
}

u8 UxROM::GetPRGBanks() {
    return m_prgBanks;
}

u8 UxROM::GetPRGBank0() {
    return m_prgBank;
}

u8 UxROM::GetPRGBank1() {
    return m_prgBanks - 1;
}

u8 UxROM::GetCHRBanks() {
    return m_chrBanks;
}

u8 UxROM::GetCHRBank0() {
    return 0;
}

u8 UxROM::GetCHRBank1() {
    return 1;
}

void UxROM::SaveState(std::ostream* stream) {}
void UxROM::LoadState(std::istream* stream) {}

void UxROM::Extract() {}
