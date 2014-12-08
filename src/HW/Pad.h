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

#ifndef __PAD_H__
#define __PAD_H__

#include "Def.h"

enum e_smspad { UP, DOWN, LEFT, RIGHT, B1, B2 };

class Pad {
public:
    Pad();
    
    void SetButtonsStatePad1(bool buttonsState[6]);
    void SetButtonsStatePad2(bool buttonsState[6]);
    void SetPauseState(bool state);
    bool PauseInterrupt();
    u8   GetData(u8 port);              //0xDC, 0xDD
    void SetControl(u8 value);
    void SetRegionData(u8 value);
    void SetGGMode(bool value);
    
    void SetSDSCControl(u8 value);
    void SetSDSCData(u8 value);
    
private:
    bool m_buttonsStatePad1[6];
    bool m_buttonsStatePad2[6];
    bool m_pauseState;
    bool m_interrupt;
    bool m_enabled;
    bool m_GameGear;
    
    u8  m_data1, m_data2;
};

#endif
