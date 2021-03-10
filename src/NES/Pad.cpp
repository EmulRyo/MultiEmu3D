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
    for (int i=0; i<6; i++) {
        m_buttonsStatePad1[i] = false;
        m_buttonsStatePad2[i] = false;
    }
}

void Pad::SetButtonsStatePad1(bool buttonsState[6]) {
	for (int i=0; i<6; i++)
		m_buttonsStatePad1[i] = buttonsState[i];
}

void Pad::SetButtonsStatePad2(bool buttonsState[6]) {
    for (int i=0; i<6; i++)
        m_buttonsStatePad2[i] = buttonsState[i];
}

u8 Pad::MemR(u16 address) {
    return 0;
}

void Pad::MemW(u16 address, u8 value) {
    
}
