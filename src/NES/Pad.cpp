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

#include <iostream>
#include "Pad.h"
#include "../Common/Bit.h"

using namespace Nes;

Pad::Pad() {
    m_strobe = 1;
    m_shiftRegister[0] = 0;
    m_shiftRegister[1] = 0;
    for (int i=0; i<8; i++) {
        m_buttonsStatePad[0][i] = false;
        m_buttonsStatePad[1][i] = false;
    }
}

u8 Pad::BuildShiftRegister(u8 padID) const {
    // Convert the UI button array into the byte latched by the controller when
    // the CPU raises the strobe line through $4016.
    u8 value = 0;
    value |= m_buttonsStatePad[padID][(int)PadButtons::A]      ? 0x01 : 0x00;
    value |= m_buttonsStatePad[padID][(int)PadButtons::B]      ? 0x02 : 0x00;
    value |= m_buttonsStatePad[padID][(int)PadButtons::SELECT] ? 0x04 : 0x00;
    value |= m_buttonsStatePad[padID][(int)PadButtons::START]  ? 0x08 : 0x00;
    value |= m_buttonsStatePad[padID][(int)PadButtons::UP]     ? 0x10 : 0x00;
    value |= m_buttonsStatePad[padID][(int)PadButtons::DOWN]   ? 0x20 : 0x00;
    value |= m_buttonsStatePad[padID][(int)PadButtons::LEFT]   ? 0x40 : 0x00;
    value |= m_buttonsStatePad[padID][(int)PadButtons::RIGHT]  ? 0x80 : 0x00;
    return value;
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
    u8 padID = 0;

    if (address == 0x4016)
        padID = 0;
    else if (address == 0x4017)
        padID = 1;

    if (m_strobe)
        // While strobe is high, the controller continually reloads internally,
        // so every read observes the current A button state.
        return BuildShiftRegister(padID) & 0x01;

    // With strobe low, the controller behaves as a serial shift register. After
    // the 8 real buttons are consumed, official pads keep returning 1.
    u8 value = m_shiftRegister[padID] & 0x01;
    m_shiftRegister[padID] = (m_shiftRegister[padID] >> 1) | 0x80;

    return value;
}

void Pad::MemW(u16 address, u8 value) {
    if (address == 0x4016) {
        m_strobe = value & 0x01;
        if (m_strobe) {
            // Latch both controllers on strobe high. The following strobe-low
            // reads shift out this stable snapshot instead of live button state.
            m_shiftRegister[0] = BuildShiftRegister(0);
            m_shiftRegister[1] = BuildShiftRegister(1);
        }
    }
}
