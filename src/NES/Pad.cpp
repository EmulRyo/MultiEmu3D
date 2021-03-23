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

#include <iostream>
#include "Pad.h"
#include "../Common/Bit.h"

using namespace Nes;

Pad::Pad() {
    m_parallelControl = 1;
    m_serialNumber[0] = 0;
    m_serialNumber[1] = 0;
    for (int i=0; i<8; i++) {
        m_buttonsStatePad[0][i] = false;
        m_buttonsStatePad[1][i] = false;
    }
}

void Pad::SetButtonsStatePad1(bool buttonsState[8]) {
	for (int i=0; i<8; i++)
		m_buttonsStatePad[0][i] = buttonsState[i];
}

void Pad::SetButtonsStatePad2(bool buttonsState[8]) {
    for (int i=0; i<8; i++)
        m_buttonsStatePad[1][i] = buttonsState[i];
}

u8 Pad::MemR(u16 address) {
    u8 value = 0;
    u8 padID = 0;

    if (address == 0x4016)
        padID = 0;
    else if (address == 0x4017)
        padID = 1;

    switch (m_serialNumber[padID]) {
        case 0: value = m_buttonsStatePad[padID][(int)PadButtons::A] == true ? 1 : 0; break;
        case 1: value = m_buttonsStatePad[padID][(int)PadButtons::B] == true ? 1 : 0; break;
        case 2: value = m_buttonsStatePad[padID][(int)PadButtons::SELECT] == true ? 1 : 0; break;
        case 3: value = m_buttonsStatePad[padID][(int)PadButtons::START] == true ? 1 : 0; break;
        case 4: value = m_buttonsStatePad[padID][(int)PadButtons::UP] == true ? 1 : 0; break;
        case 5: value = m_buttonsStatePad[padID][(int)PadButtons::DOWN] == true ? 1 : 0; break;
        case 6: value = m_buttonsStatePad[padID][(int)PadButtons::LEFT] == true ? 1 : 0; break;
        case 7: value = m_buttonsStatePad[padID][(int)PadButtons::RIGHT] == true ? 1 : 0; break;
        case 8: value = 1; break;
        default: value = 0; break;
    }

    if (m_parallelControl == 0) {
        if (m_serialNumber[padID] < 8)
            m_serialNumber[padID]++;
    }

    return value;
}

void Pad::MemW(u16 address, u8 value) {
    if (address == 0x4016) {
        m_parallelControl = value & 0x01;
        m_serialNumber[0] = 0;
        m_serialNumber[1] = 0;
    }
}
