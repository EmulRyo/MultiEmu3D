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

#ifndef __NES_INSTRUCTIONS_H__
#define __NES_INSTRUCTIONS_H__

namespace Nes {
    
    class Registers;
    class Memory;

    class Instructions
    {
    public:
        Instructions(Registers *reg, Memory *mem);
        ~Instructions(void);

        u8  Get8BitsInmValue();
        u16 Get16BitsInmValue();

        void BMI();
        void BPL();
        void BCC();
        void BCS();
        void BEQ();
        void BNE();
        void BVS();
        void BVC();

        void CMP(u8 value, u8 length);
        void CPX(u8 value, u8 length);
        void CPY(u8 value, u8 length);

        void JMP();
        void JMPIndirect();

        void ORA(u8 value, u8 length);
        void PHP();
        void PHA();
        void PLP();
        void PLA();
        
    private:
        Registers   *m_reg;
        Memory      *m_mem;
    };
}

#endif
