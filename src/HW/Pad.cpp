/*
 This file is part of DMGBoy.
 
 DMGBoy is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 DMGBoy is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with DMGBoy.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Pad.h"

enum e_smspad { UP, DOWN, LEFT, RIGHT, B1, B2 };

Pad::Pad() {
    for (int i=0; i<6; i++) {
        m_buttonsStatePad1[i] = false;
        m_buttonsStatePad2[i] = false;
    }
    
    m_data1 = 0xFF;
    m_data2 = 0xFF;
}

void Pad::SetButtonsStatePad1(bool buttonsState[6]) {
	for (int i=0; i<6; i++)
		m_buttonsStatePad1[i] = buttonsState[i];
	
	m_data1 = (m_data1 & 0xC0);
    m_data1 |= m_buttonsStatePad1[UP]    ? 0x00 : 0x01;
    m_data1 |= m_buttonsStatePad1[DOWN]  ? 0x00 : 0x02;
    m_data1 |= m_buttonsStatePad1[LEFT]  ? 0x00 : 0x04;
    m_data1 |= m_buttonsStatePad1[RIGHT] ? 0x00 : 0x08;
    m_data1 |= m_buttonsStatePad1[B1]    ? 0x00 : 0x10;
    m_data1 |= m_buttonsStatePad1[B2]    ? 0x00 : 0x20;
}

u8 Pad::GetData(u8 port) {
    if (port == 0xDC)
        return m_data1;
    else
        return m_data2;
}

void Pad::SetData(u8 port, u8 value) {
    
}
