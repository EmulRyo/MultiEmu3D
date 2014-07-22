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

#ifndef __INSTRUCCIONES_H__
#define __INSTRUCCIONES_H__

class Registers;
class Memory;

class Instructions
{
public:
	Instructions(Registers *reg, Memory *mem);
	~Instructions(void);
	
	void ADC_A_n(e_registers place);
	void ADD_A_n(e_registers place);
	void ADD_HL_n(e_registers place);
	void AND(e_registers place);
	void BIT_b_r(u8 bit, e_registers place);
	void CALL_nn();
	void CALL_cc_nn(e_registers flag, u8 value2check);
	void CP_n(e_registers place);
	void CPL();
	void DAA();
	void DEC_n(e_registers place);
    void DEC_nn(e_registers place);
	void DI();
	void EI();
	void HALT();
	void INC_n(e_registers place);
    void INC_nn(e_registers place);
	void JP_nn();
	void JP_cc_nn(e_registers flag, u8 value2check);
	void JP_HL();
	void JR();
    void JR_CC_n(e_registers flag, u8 value2check);
	void LD_n_A(e_registers place);
    void LD_n_nn(e_registers place);
	void LD_nn_n(e_registers place);
	void LD_A_n(e_registers place);
	void LD_r1_r2(e_registers e_reg1, e_registers e_reg2);
	void LD_SP_HL();
	void NOP();
    void OR_n(e_registers place);
	void POP_nn(e_registers place);
    void PUSH_nn(e_registers place);
	void RES_b_r(u8 bit, e_registers place);
	void RET();
	void RETI();
    void RET_cc(e_registers flag, u8 value2check);
	void RL_n(e_registers place);
	//void RLA();
	void RLCA();
	void RLC_n(e_registers place);
	void RR_n(e_registers place);
	void RRC_n(e_registers place);
	//void RRCA();
	void RST_n(u8 desp);
    void SBC_A(e_registers place);
	void SET_b_r(u8 bit, e_registers place);
	void SCF();
    void SLA_n(e_registers place);
	void SRA_n(e_registers place);
    void SRL_n(e_registers place);
	void SUB_n(e_registers place);
	void XOR_n(e_registers place);
    
    void IM(u8 mode);
    void DJNZ();
    void LD_cNN_nn(e_registers place);
    void LD_cNN_n(e_registers place);
    void LDI();
    void LDIR();
    void OUT(e_registers placePort, e_registers placeValue);
    void OUTI();
    void OTIR();
    void SLL_n(e_registers place);
    void CCF();
    void IN_A_n();
    void IN(e_registers place);
    void SBC_HL(e_registers place);
    void LD_R_A();
    void LD_A_R();
    void EX_DE_HL();
    void EX_AF_AF2();
    void EXX();
    void EX_cSP_HL();
    void PUSH(u16 value);
    void POP(u16 *reg);
    void LD_HL_cNN();
    void ADD(u16 *reg, u16 value);
    void LD(u8  *reg, u8  value);
    void LD(u16 *reg, u16 value);
    void DEC(u8 *reg);
    void CPI();
    
private:
	Registers   *m_reg;
	Memory      *m_mem;
    u8          m_opcode;
    
    u8 EvenBitsSet(u8 v);
};

#endif
