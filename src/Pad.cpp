/*
 This file is part of gbpablog.
 
 gbpablog is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 gbpablog is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with gbpablog.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <wx/wx.h>
#include <iostream>
#include "Pad.h"
#include "GBException.h"
using namespace std;

enum e_gbpad { gbRIGHT, gbLEFT, gbUP, gbDOWN, gbA, gbB, gbSELECT, gbSTART };

static wxKeyCode keysUsed[] = { WXK_RIGHT, WXK_LEFT, WXK_UP, WXK_DOWN, (wxKeyCode)'A', (wxKeyCode)'S', (wxKeyCode)'Q', (wxKeyCode)'W' };

static BYTE gbPadState[8];

BYTE updateInput(BYTE valueP1)
{
	if(!BIT5(valueP1))
		return ((valueP1 & 0x30) |
			(!gbPadState[gbSTART] << 3) | (!gbPadState[gbSELECT] << 2) | (!gbPadState[gbB] << 1) | (!gbPadState[gbA]));

	if(!BIT4(valueP1))
		return ((valueP1 & 0x30) |
			(!gbPadState[gbDOWN] << 3) | (!gbPadState[gbUP] << 2) | (!gbPadState[gbLEFT] << 1) | (!gbPadState[gbRIGHT]));

	//Desactivar los botones
	return 0x3F;
}

// Devuelve 1 cuando se ha pulsado una tecla
// 0 en caso contrario
int checkKeyboard(BYTE * valueP1)
{
	
	int interrupt = 0;
	
	for (int i=0; i<8; i++)
	{
		if ((gbPadState[i] == 0) && (wxGetKeyState(keysUsed[i]) == true))
		{
			interrupt = 1;
		}
		
		gbPadState[i] = wxGetKeyState(keysUsed[i]);
	}
	
	*valueP1 = updateInput(*valueP1);
	
	return 0;
}
