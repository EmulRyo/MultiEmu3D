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

class Pad {
public:
    Pad();
    
    void SetButtonsStatePad1(bool buttonsState[6]);
    void SetButtonsStatePad2(bool buttonsState[6]);
    u8   GetData(u8 port);              //0xDC, 0xDD
    void SetRegionData(u8 value);
    
private:
    bool m_buttonsStatePad1[6];
    bool m_buttonsStatePad2[6];
    
    u8  m_data1, m_data2;
};

#endif
