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
    m_pauseState = false;
    m_interrupt  = false;
    m_enabled    = true;
    
    m_data1 = 0xFF;
    m_data2 = 0xFF;
}

void Pad::SetGGMode(bool value) {
    m_GameGear = value;
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

void Pad::SetButtonsStatePad2(bool buttonsState[6]) {
    for (int i=0; i<6; i++)
        m_buttonsStatePad2[i] = buttonsState[i];
    
    m_data1 = (m_data1 & 0x3F);
    m_data1 |= m_buttonsStatePad2[UP]    ? 0x00 : 0x40;
    m_data1 |= m_buttonsStatePad2[DOWN]  ? 0x00 : 0x80;
    
    m_data2 = (m_data2 & 0xF0);
    m_data2 |= m_buttonsStatePad2[LEFT]  ? 0x00 : 0x01;
    m_data2 |= m_buttonsStatePad2[RIGHT] ? 0x00 : 0x02;
    m_data2 |= m_buttonsStatePad2[B1]    ? 0x00 : 0x04;
    m_data2 |= m_buttonsStatePad2[B2]    ? 0x00 : 0x08;
}

void Pad::SetPauseState(bool state) {
    m_interrupt = (state && (!m_pauseState));
    m_pauseState = state;
}

bool Pad::PauseInterrupt() {
    if (m_GameGear)
        return false;
    else {
        bool value = m_interrupt;
        m_interrupt = false;
        return value;
    }
}

u8 Pad::GetData(u8 port) {
    if (port == 0)
        return m_pauseState ? 0x40 : 0xC0;
        
    //if (!m_enabled)
    //    return 0xFF;
    
    port &= 0xC1;
    if (port == 0xC0)
        return m_data1;
    else
        return m_data2;
}

void Pad::SetRegionData(u8 value) {
    if (value & 0x04)
        m_data2 = (m_data2 & 0x7F) | (value & 0x80);
    else
        m_data2 = (m_data2 & 0x7F) | 0x80;
    
    if (value & 0x01)
        m_data2 = (m_data2 & 0xBF) | ((value & 0x20) << 1);
    else
        m_data2 = (m_data2 & 0xBF) | 0x20;
}

void Pad::SetControl(u8 value) {
    m_enabled = BIT2(value) ? false : true;
}

void Pad::SetSDSCControl(u8 value) {
    
}

void Pad::SetSDSCData(u8 value) {
    if (!m_enabled)
        printf("%c", value);
}
