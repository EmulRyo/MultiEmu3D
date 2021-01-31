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

#ifndef __INSTRUCCIONES_H__
#define __INSTRUCCIONES_H__

namespace MasterSystem {
    
    class Registers;
    class Memory;

    class Instructions
    {
    public:
        Instructions(Registers *reg, Memory *mem);
        ~Instructions(void);
        
        void CALL_nn();
        void CALL_cc_nn(e_registers flag, u8 value2check);
        void CPL();
        void DAA();
        void DI();
        void EI();
        void HALT();
        void JP_cc_nn(e_registers flag, u8 value2check);
        void JR();
        void JR_CC_n(e_registers flag, u8 value2check);
        void NOP();
        void RES_b_r(u8 bit, e_registers place);
        void RET();
        void RETI();
        void RET_cc(e_registers flag, u8 value2check);
        void RST_n(u8 desp);
        void SET_b_r(u8 bit, e_registers place);
        void SCF();
        
        void IM(u8 mode);
        void DJNZ();
        void LDI();
        void LDIR();
        void OUT(u8 port, u8 value);
        void OUTI();
        void OTIR();
        void CCF();
        void IN_A_n();
        void IN(u8 *reg);
        void IN_OnlyFlags();
        void LD_A_R();
        void EX_DE_HL();
        void EX_AF_AF2();
        void EXX();
        void PUSH(u16 value);
        void POP(u16 *reg);
        void LD_HL_cNN();
        void ADD(u16 *reg, u16 value);
        void LD(u8  *reg, u8 value);
        void LD(u16 *reg, u16 value);
        void LD_Content(u16 *reg, u16 address);
        void CPI();
        void NEG();
        void LD_Mem(u16 address, u8 value);
        void LD_Mem(u16 address, u16 value);
        void BIT(u8 bit, u8 value);
        void BIT(u8 bit, u16 address);
        void BIT_HL(u8 bit);
        void SET_Mem(u8 bit, u16 address);
        void RES_Mem(u8 bit, u16 address);
        void LDD();
        void LDDR();
        void OR(u8 value);
        void ADD(u8 value);
        void INC_NoFlags(u16 *reg);
        void INC(u8 *reg);
        void INC_Mem(u16 address);
        void DEC_NoFlags(u16 *reg);
        void DEC(u8 *reg);
        void DEC_Mem(u16 address);
        void ADC(u8 value);
        void ADC(u16 value);
        void CP(u8 value);
        void SUB(u8 value);
        void OUTD();
        void OTDR();
        void RLCA();
        void RLC(u8 *reg);
        void RLC_Mem(u16 address);
        void RLC_Mem(u8 *reg, u16 address);
        void AND(u8 value);
        void XOR(u8 value);
        void RRCA();
        void RRC(u8 *reg);
        void RRC_Mem(u16 address);
        void RRC_Mem(u8 *reg, u16 address);
        void JP(u16 value);
        void SBC(u8  value);
        void SBC(u16 value);
        void EX_cSP(u16 *value);
        void RLD();
        void RRD();
        void RLA();
        void RL(u8 *reg);
        void RL_Mem(u16 address);
        void RL_Mem(u8 *reg, u16 address);
        void RRA();
        void RR(u8 *reg);
        void RR_Mem(u16 address);
        void RR_Mem(u8 *reg, u16 address);
        void SRA(u8 *reg);
        void SRA_Mem(u16 address);
        void SRA_Mem(u8 *reg, u16 address);
        void SLA(u8 *reg);
        void SLA_Mem(u16 address);
        void SLA_Mem(u8 *reg, u16 address);
        void SLL(u8 *reg);
        void SLL_Mem(u16 address);
        void SLL_Mem(u8 *reg, u16 address);
        void SRL(u8 *reg);
        void SRL_Mem(u16 address);
        void SRL_Mem(u8 *reg, u16 address);
        void INI();
        void INIR();
        void CPIR();
        void CPD();
        void CPDR();
        void LD_A_I();
        void RETN();
        
    private:
        Registers   *m_reg;
        Memory      *m_mem;
        
        u8 EvenBitsSet(u8 v);
        u8 OverflowAddition(u8 v1, u8 v2);
        u8 OverflowAddition(u16 v1, u16 v2);
        u8 OverflowSubstraction(u8 v1, u8 v2);
        u8 OverflowSubstraction(u16 v1, u16 v2);
    };
}

#endif
