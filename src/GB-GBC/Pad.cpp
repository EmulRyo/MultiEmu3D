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

#include "Pad.h"
#include "../Common/Bit.h"

using namespace GameBoy;

Pad::Pad() {
    for (int i=0; i<8; i++)
        m_buttonsState[i] = false;
}

u8 Pad::Update(u8 valueP1)
{
    u8 newValue = 0;
    
	if(!BIT5(valueP1)) {
        u8 start  = (m_buttonsState[START]  ? 0 : 1) << 3;
        u8 select = (m_buttonsState[SELECT] ? 0 : 1) << 2;
        u8 b      = (m_buttonsState[BB]      ? 0 : 1) << 1;
        u8 a      = (m_buttonsState[BA]      ? 0 : 1);
		newValue = start | select | b | a;
    }
    else if(!BIT4(valueP1)) {
        u8 down  = (m_buttonsState[DOWN]  ? 0 : 1) << 3;
        u8 up    = (m_buttonsState[UP]    ? 0 : 1) << 2;
        u8 left  = (m_buttonsState[LEFT]  ? 0 : 1) << 1;
        u8 right = (m_buttonsState[RIGHT] ? 0 : 1);
		newValue = down | up | left | right;
    }
    else {
        //Desactivar los botones
        newValue = 0x0F;
    }
	return ((valueP1 & 0xF0) | newValue);
}

// Devuelve 1 cuando se ha pulsado un botón
// 0 en caso contrario
int Pad::SetButtonsState(bool buttonsState[8], u8 *valueP1)
{
	
	int interrupt = 0;
	
	for (int i=0; i<8; i++)
	{
		if ((m_buttonsState[i] == 0) && (buttonsState[i] == true))
		{
			interrupt = 1;
		}
		
		m_buttonsState[i] = buttonsState[i];
	}
	
	*valueP1 = Update(*valueP1);
	
	return interrupt;
}
