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
#include "NROM.h"

using namespace std;
using namespace Nes;

NROM::NROM(u8* buffer)
    :Mapper(buffer){

}

void NROM::Reset() {}

NametableMirroring NROM::GetNametableMirroring() { return m_hardWireMirroring; }

u8 NROM::ReadPRG(u16 address) {
    if (address < 0x8000)
        return m_prgData[address - 0x8000];
    else if (address < 0xC000)
        return m_prgData[address - 0x8000];
    else {
        if (m_prgBanks == 2)
            return m_prgData[address - 0x8000];
        else
            return m_prgData[address - 0xC000];
    }
}

void NROM::WritePRG(u16 address, u8 value) {

}

u8 NROM::ReadCHR(u16 address) {
	return m_chrData[address];
}

void NROM::WriteCHR(u16 address, u8 value) {

}

u8 NROM::GetMapperNum() {
    return 0;
}

const char* NROM::GetMapperName() {
    return "NROM";
}

u8 NROM::GetPRGBanks() {
    return m_prgBanks;
}

u8 NROM::GetPRGBank0() {
    return 0;
}

u8 NROM::GetPRGBank1() {
    return (m_prgBanks == 2) ? 1 : 0;
}

u8 NROM::GetCHRBanks() {
    return 2;
}

u8 NROM::GetCHRBank0() {
    return 0;
}

u8 NROM::GetCHRBank1() {
    return 1;
}

void NROM::SaveState(std::ostream* stream) {}
void NROM::LoadState(std::istream* stream) {}

void NROM::Extract() {}
