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

#ifndef __NES_PAD_H__
#define __NES_PAD_H__

#include "../Common/Types.h"

namespace Nes {

    enum e_smspad { UP, DOWN, LEFT, RIGHT, B1, B2 };

    class Pad {
    public:
        Pad();
        
        void SetButtonsStatePad1(bool buttonsState[6]);
        void SetButtonsStatePad2(bool buttonsState[6]);
        u8   MemR(u16 address);
        void MemW(u16 address, u8 value);
        
    private:
        bool m_buttonsStatePad1[6];
        bool m_buttonsStatePad2[6];
    };
}

#endif
