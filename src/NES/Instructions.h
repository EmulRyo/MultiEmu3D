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

        u8   GetCyclesExtra();
        void ResetCyclesExtra();

        u8  Get8BitsInmValue();
        u16 Get16BitsInmValue();

        void ADC(u8 value, u8 length);
        void AND(u8 value, u8 length);
        void ASL();
        void ASL(u16 address, u8 length);

        void BIT(u8 value, u8 length);
        void BMI();
        void BPL();
        void BCC();
        void BCS();
        void BEQ();
        void BNE();
        void BRK();
        void BVS();
        void BVC();

        void CLC();
        void CLD();
        void CLI();
        void CLV();

        void DEC(u16 address, u8 length);
        void DEX();
        void DEY();

        void EOR(u8 value, u8 length);

        void CMP(u8 value, u8 length);
        void CPX(u8 value, u8 length);
        void CPY(u8 value, u8 length);

        void INC(u16 address, u8 length);
        void INX();
        void INY();
        void IRQ(u16 addressToStore, u8 stackBits45, u16 indAddressToJump);

        void JMP();
        void JMPIndirect();
        void JSR();

        void LDA(u8 value, u8 length);
        void LDX(u8 value, u8 length);
        void LDY(u8 value, u8 length);
        void LSR();
        void LSR(u16 address, u8 length);

        void NMI();
        void NOP(u8 length);

        void ORA(u8 value, u8 length);

        void PHP();
        void PHA();
        void PLP();
        void PLA();

        void ROL();
        void ROL(u16 address, u8 length);
        void ROR();
        void ROR(u16 address, u8 length);
        void RTI();
        void RTS();

        void SBC(u8 value, u8 length);
        void SEC();
        void SED();
        void SEI();
        void STA(u16 address, u8 length);
        void STX(u16 address, u8 length);
        void STY(u16 address, u8 length);

        void TAX();
        void TAY();
        void TSX();
        void TXA();
        void TXS();
        void TYA();
        
    private:
        Registers   *m_reg;
        Memory      *m_mem;
        u8           m_cyclesExtra;
    };
}

#endif
