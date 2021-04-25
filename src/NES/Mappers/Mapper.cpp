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
#include "Mapper.h"
#include "NROM.h"
#include "MMC1.h"
#include "UxROM.h"

using namespace std;
using namespace Nes;

#define SIZE_PRG_ROM 4
#define SIZE_CHR_ROM 5

Mapper* Nes::Mapper::Create(u16 mapperId, u8* buffer)
{
	switch (mapperId) {
		case 0: return new NROM(buffer);
        case 1: return new MMC1(buffer);
        case 2: return new UxROM(buffer);
		default: return nullptr;
	}
}

Mapper::Mapper(u8* buffer) {
    m_prgBanks = buffer[SIZE_PRG_ROM];
    m_chrBanks = buffer[SIZE_CHR_ROM];
    size_t sizePrgRom = m_prgBanks * 16384ULL;
    size_t sizeChrRom = m_chrBanks * 8192ULL;

    m_hardWireMirroring = BIT0(buffer[6]) ? NametableMirroring::VERTICAL : NametableMirroring::HORIZONTAL;
    if (BIT3(buffer[6]) > 0)
        m_hardWireMirroring = NametableMirroring::FOUR_SCREEN;
    m_mapperMirroring = m_hardWireMirroring;

    bool battery = (BIT1(buffer[6]) > 0) ? true : false;
    u8   trainer = (BIT2(buffer[6]) > 0) ? 1 : 0;


    bool VSUnisystem = ((buffer[7] & 0x01) > 0) ? true : false;
    bool playChoice10 = ((buffer[7] & 0x02) > 0) ? true : false;

    size_t headerSize = 16;
    size_t trainerSize = 512;
    m_prgData = buffer + headerSize + trainer * trainerSize;
    m_chrData = m_prgData + sizePrgRom;
}
