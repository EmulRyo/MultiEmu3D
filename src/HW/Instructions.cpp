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

#include <iostream>
#include <assert.h>
#include "SMSException.h"
#include "Registers.h"
#include "Memory.h"
#include "Instructions.h"

using namespace std;

#define _8bitsInmValue (m_mem->MemR(m_reg->GetPC() + 1))
#define _16bitsInmValue ((m_mem->MemR(m_reg->GetPC() + 2)) << 8) | m_mem->MemR(m_reg->GetPC() + 1)

Instructions::Instructions(Registers* reg, Memory* mem)
{
	m_reg = reg;
	m_mem = mem;
    m_opcode = 0x00;
}

Instructions::~Instructions(void)
{
}

void Instructions::NOP() { }

void Instructions::LD_r1_r2(e_registers e_reg1, e_registers e_reg2)
{
	if (e_reg1 == c_HL)
	{
		if (e_reg2 == $)
			m_mem->MemW(m_reg->GetHL(), _8bitsInmValue);
		else
			m_mem->MemW(m_reg->GetHL(), (u8)m_reg->GetReg(e_reg2));
	}
	else
	{
		if (e_reg2 == c_HL)
			m_reg->SetReg(e_reg1, m_mem->MemR(m_reg->GetHL()));
		else
			m_reg->SetReg(e_reg1, m_reg->GetReg(e_reg2));
	}
}


void Instructions::LD_A_n(e_registers place)
{
    int address, value;

	switch(place)
	{
		case $:
			value = _8bitsInmValue;
			break;
		case c_$$:
			address = _16bitsInmValue;
			value = m_mem->MemR(address);
			break;
		case c_BC:
			value = m_mem->MemR(m_reg->GetBC());
			break;
		case c_DE:
			value = m_mem->MemR(m_reg->GetDE());
			break;
		case c_HL:
			value = m_mem->MemR(m_reg->GetHL());
			break;
		default:
			value = m_reg->GetReg(place);
	}

	m_reg->SetA(value);
}


void Instructions::LD_n_A(e_registers place)
{
    int address;

	switch (place)
	{
		case c_$$:
			address = _16bitsInmValue;
			m_mem->MemW(address, m_reg->GetA());
			break;
		case c_BC:
			m_mem->MemW(m_reg->GetBC(), m_reg->GetA());
			break;
		case c_DE:
			m_mem->MemW(m_reg->GetDE(), m_reg->GetA());
			break;
		case c_HL:
			m_mem->MemW(m_reg->GetHL(), m_reg->GetA());
			break;
		default:
			m_reg->SetReg(place, m_reg->GetA());
	}
}

void Instructions::CPL()
{
	m_reg->SetA(~m_reg->GetA());

	m_reg->SetFlagN(1);
	m_reg->SetFlagH(1);
}

void Instructions::LD_n_nn(e_registers place)
{
	assert((place == BC) || (place == DE) || (place == HL) || (place == SP));
	m_reg->SetReg(place, _16bitsInmValue);
}

void Instructions::JR()
{
    char address = _8bitsInmValue; //Con signo

	//El "2 +" es porque antes de saltar ha tenido que ver cuales eran los dos opcodes de la instruccion
	//y tiene importancia al ser un salto relativo con respecto al actual PC.
    m_reg->AddPC(2 + address);
    m_reg->SetIncPC(false);
}

void Instructions::JR_CC_n(e_registers flag, u8 value2check)
{
	if (m_reg->GetFlag(flag) == value2check) {
		JR();
        m_reg->SetConditionalTaken(true);
    }
}

void Instructions::CALL_nn()
{
	m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(),((m_reg->GetPC() + 3) & 0xFF00) >> 8);
	m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(),(m_reg->GetPC() + 3) & 0x00FF);
	m_reg->SetPC(_16bitsInmValue);
    m_reg->SetIncPC(false);
}

void Instructions::CALL_cc_nn(e_registers flag, u8 value2check)
{
	if (m_reg->GetFlag(flag) == value2check) {
		CALL_nn();
        m_reg->SetConditionalTaken(true);
    }
}

void Instructions::LD_SP_HL()
{
	m_reg->SetSP(m_reg->GetHL());
}

void Instructions::ADD_HL_n(e_registers place)
{
	u16 value, hl;
	
	value = m_reg->GetReg(place);
    hl = m_reg->GetHL();
	
	m_reg->SetFlagN(0);
	m_reg->SetFlagH((((hl & 0x0FFF) + (value & 0x0FFF)) > 0x0FFF) ? 1 : 0);
	m_reg->SetFlagC(((hl + value) > 0xFFFF) ? 1 : 0);
	
    m_reg->SetHL(hl + value);
}

void Instructions::DAA()
{
	/*
	 http://www.emutalk.net/showthread.php?t=41525&page=108
	 
	 Detailed info DAA
	 Instruction Format:
	 OPCODE                    CYCLES
	 --------------------------------
	 27h                       4
	 
	 
	 Description:
	 This instruction conditionally adjusts the accumulator for BCD addition
	 and subtraction operations. For addition (ADD, ADC, INC) or subtraction
	 (SUB, SBC, DEC, NEC), the following table indicates the operation performed:
	 
	 --------------------------------------------------------------------------------
	 |           | C Flag  | HEX value in | H Flag | HEX value in | Number  | C flag|
	 | Operation | Before  | upper digit  | Before | lower digit  | added   | After |
	 |           | DAA     | (bit 7-4)    | DAA    | (bit 3-0)    | to u8 | DAA   |
	 |------------------------------------------------------------------------------|
	 |           |    0    |     0-9      |   0    |     0-9      |   00    |   0   |
	 |   ADD     |    0    |     0-8      |   0    |     A-F      |   06    |   0   |
	 |           |    0    |     0-9      |   1    |     0-3      |   06    |   0   |
	 |   ADC     |    0    |     A-F      |   0    |     0-9      |   60    |   1   |
	 |           |    0    |     9-F      |   0    |     A-F      |   66    |   1   |
	 |   INC     |    0    |     A-F      |   1    |     0-3      |   66    |   1   |
	 |           |    1    |     0-2      |   0    |     0-9      |   60    |   1   |
	 |           |    1    |     0-2      |   0    |     A-F      |   66    |   1   |
	 |           |    1    |     0-3      |   1    |     0-3      |   66    |   1   |
	 |------------------------------------------------------------------------------|
	 |   SUB     |    0    |     0-9      |   0    |     0-9      |   00    |   0   |
	 |   SBC     |    0    |     0-8      |   1    |     6-F      |   FA    |   0   |
	 |   DEC     |    1    |     7-F      |   0    |     0-9      |   A0    |   1   |
	 |   NEG     |    1    |     6-F      |   1    |     6-F      |   9A    |   1   |
	 |------------------------------------------------------------------------------|
	 
	 
	 Flags:
	 C:   See instruction.
	 N:   Unaffected.
	 P/V: Set if Acc. is even parity after operation, reset otherwise.
	 H:   See instruction.
	 Z:   Set if Acc. is Zero after operation, reset otherwise.
	 S:   Set if most significant bit of Acc. is 1 after operation, reset otherwise.
	 
	 Example:
	 
	 If an addition operation is performed between 15 (BCD) and 27 (BCD), simple decimal
	 arithmetic gives this result:
	 
	 15
	 +27
	 ----
	 42
	 
	 But when the binary representations are added in the Accumulator according to
	 standard binary arithmetic:
	 
	 0001 0101  15
	 +0010 0111  27
	 ---------------
	 0011 1100  3C
	 
	 The sum is ambiguous. The DAA instruction adjusts this result so that correct
	 BCD representation is obtained:
	 
	 0011 1100  3C result
	 +0000 0110  06 +error
	 ---------------
	 0100 0010  42 Correct BCD!
	*/
    
    int a = m_reg->GetA();
    
    if (m_reg->GetFlagN() == 0)
    {
        if (m_reg->GetFlagH() || ((a & 0xF) > 9))
            a += 0x06;
        
        if (m_reg->GetFlagC() || (a > 0x9F))
            a += 0x60;
    }
    else
    {
        if (m_reg->GetFlagH())
            a = (a - 6) & 0xFF;
        
        if (m_reg->GetFlagC())
            a -= 0x60;
    }
    
    m_reg->SetFlagH(0);
    m_reg->SetFlagZ(0);
    
    if ((a & 0x100) == 0x100)
        m_reg->SetFlagC(1);
    
    a &= 0xFF;
    
    if (a == 0)
        m_reg->SetFlagZ(1);
    
    m_reg->SetA(a);
}

void Instructions::RET()
{
	m_reg->SetPC((m_mem->MemR(m_reg->GetSP() + 1) << 8) | m_mem->MemR(m_reg->GetSP()));
	m_reg->AddSP(2);
    m_reg->SetIncPC(false);
}

void Instructions::RETI()
{
	EI();
	RET();
}

void Instructions::RET_cc(e_registers flag, u8 value2check)
{
	if (m_reg->GetFlag(flag) == value2check) {
		RET();
        m_reg->SetConditionalTaken(true);
    }
}

void Instructions::LD_nn_n(e_registers place)
{
	m_reg->SetReg(place, _8bitsInmValue);
}

void Instructions::SET_b_r(u8 bit, e_registers place)
{
	if (place == c_HL)
		m_mem->MemW(m_reg->GetHL(), m_mem->MemR(m_reg->GetHL()) | (1 << bit));
	else
		m_reg->SetReg(place, m_reg->GetReg(place) | (1 << bit));
}

void Instructions::RES_b_r(u8 bit, e_registers place)
{
    if (place == c_HL)
		m_mem->MemW(m_reg->GetHL(), m_mem->MemR(m_reg->GetHL()) & ~(1 << bit));
	else
		m_reg->SetReg(place, m_reg->GetReg(place) & ~(1 << bit));
}

void Instructions::DI()
{
	m_reg->SetIFF1(false);
    m_reg->SetIFF2(false);
}

void Instructions::EI()
{
	m_reg->SetIntPending(true);
}

void Instructions::SCF()
{
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(1);
}

void Instructions::HALT()
{
	m_reg->SetHalt(true);
}

void Instructions::PUSH_nn(e_registers place)
{
    m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(), (m_reg->GetReg(place) & 0xFF00) >> 8);
	m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(), m_reg->GetReg(place) & 0x00FF);
}

void Instructions::POP_nn(e_registers place)
{
	m_reg->SetReg(place, (m_mem->MemR(m_reg->GetSP() + 1) << 8) | m_mem->MemR(m_reg->GetSP()));
	m_reg->AddSP(2);
}

void Instructions::JP_cc_nn(e_registers flag, u8 value2check)
{
	if (m_reg->GetFlag(flag) == value2check) {
        u16 nn = _16bitsInmValue;
		JP(nn);
        m_reg->SetConditionalTaken(true);
    }
}

void Instructions::RST_n(u8 desp)
{
	//Queremos que se guarde la siquiente instruccion a ejecutar
	PUSH(m_reg->GetPC()+1);

	m_reg->SetPC(0x0000 + desp);
    m_reg->SetIncPC(false);
}

/*
 *
 * Nuevas instruciones
 *
 */

void Instructions::IM(u8 mode) {
    m_reg->SetIntMode(mode);
}

void Instructions::DJNZ() {
    u8 value = m_reg->GetB() - 1;
    m_reg->SetB(value);
    if (value != 0) {
        JR();
        m_reg->SetConditionalTaken(true);
    }
}

void Instructions::LD_cNN_n(e_registers place) {
    assert((place == A) || (place == B) || (place == C) ||
           (place == D) || (place == E) || (place == F));
    
    u16 address = _16bitsInmValue;
    u8  value = m_reg->GetReg(place);
    m_mem->MemW(address, value);
}

void Instructions::LDI() {
    u16 addressDE = m_reg->GetDE();
    u8 valueHL = m_mem->MemR(m_reg->GetHL());
    m_mem->MemW(addressDE, valueHL);
    m_reg->SetDE(addressDE+1);
    m_reg->SetHL(m_reg->GetHL()+1);
    m_reg->SetBC(m_reg->GetBC()-1);
    
    m_reg->SetFlagH(0);
    m_reg->SetFlagN(0);
    m_reg->SetFlagPV(!m_reg->GetBC() ? 0 : 1);

}

void Instructions::LDIR() {
    
    LDI();
    if (m_reg->GetBC()) // Si se cumple la condición evitar que salte
        m_reg->SetIncPC(false);
}

void Instructions::OUT(e_registers placePort, e_registers placeValue) {
    u8 port;
    
    if (placePort == $)
        port = _8bitsInmValue;
    else
        port = m_reg->GetReg(placePort);
	
    u8 value = (placeValue == f_Z) ? 0 : m_reg->GetReg(placeValue);
    
    m_mem->PortW(port, value);
}

void Instructions::OUTI() {
    u8 value = m_mem->MemR(m_reg->GetHL());
    m_reg->SetHL(m_reg->GetHL()+1);
    m_reg->SetB(m_reg->GetB()-1);
    u8 port = m_reg->GetC();
    
    m_mem->PortW(port, value);
    
    m_reg->SetFlagZ(m_reg->GetB() ? 0 : 1);
    m_reg->SetFlagN(1);
}

void Instructions::OTIR() {
    
    OUTI();
    if(m_reg->GetB()) // Si se cumple la condición evitar que salte
        m_reg->SetIncPC(false);
}

u8 Instructions::EvenBitsSet(u8 v) {
    u8 c; // c accumulates the total bits set in v
    for (c = 0; v; c++)
        v &= v - 1; // clear the least significant bit set
    
    return (c % 2);
}

void Instructions::CCF() {
    u8 c = m_reg->GetFlagC();
    m_reg->SetFlagH(c);
    m_reg->SetFlagN(0);
    m_reg->SetFlagC(c ? 0 : 1);
}

void Instructions::IN_A_n() {
    u8 port = _8bitsInmValue;
    u8 value = m_mem->PortR(port);
    m_reg->SetA(value);
}

void Instructions::IN(e_registers placeValue) {
    u8 port = m_reg->GetC();
    u8 value = m_mem->PortR(port);
    m_reg->SetReg(placeValue, value);
    
    s8 sValue = value;
    
    m_reg->SetFlagS((sValue < 0) ? 1 : 0);
    m_reg->SetFlagZ(!value);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
    m_reg->SetFlagN(0);
}

void Instructions::LD_R_A() {
    m_reg->SetR(m_reg->GetA());
}

void Instructions::LD_A_R() {
    u8 r = m_reg->GetR();
    m_reg->SetA(r);
    
    m_reg->SetFlagS(r >> 7);
    m_reg->SetFlagZ(r ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(m_reg->GetIFF2());
    m_reg->SetFlagN(0);
}

void Instructions::EX_DE_HL() {
    u16 de = m_reg->GetDE();
    u16 hl = m_reg->GetHL();
    
    m_reg->SetDE(hl);
    m_reg->SetHL(de);
}

void Instructions::EX_AF_AF2() {
    u16 af  = m_reg->GetAF();
    u16 af2 = m_reg->GetAF2();
    
    m_reg->SetAF(af2);
    m_reg->SetAF2(af);
}

void Instructions::EXX() {
    u16 bc = m_reg->GetBC();
    u16 de = m_reg->GetDE();
    u16 hl = m_reg->GetHL();
    u16 bc2 = m_reg->GetBC2();
    u16 de2 = m_reg->GetDE2();
    u16 hl2 = m_reg->GetHL2();
    
    m_reg->SetBC(bc2);
    m_reg->SetDE(de2);
    m_reg->SetHL(hl2);
    m_reg->SetBC2(bc);
    m_reg->SetDE2(de);
    m_reg->SetHL2(hl);
}

void Instructions::EX_cSP(u16 *value) {
    u8 h = (*value) >> 8;
    u8 l = (*value) & 0x00FF;
    u8 sp1 = m_mem->MemR(m_reg->GetSP());
    u8 sp2 = m_mem->MemR(m_reg->GetSP()+1);
    
    *value = (sp2 << 8) | sp1;
    m_mem->MemW(m_reg->GetSP(), l);
    m_mem->MemW(m_reg->GetSP()+1, h);
}

void Instructions::PUSH(u16 value)
{
    m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(), (value & 0xFF00) >> 8);
	m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(), value & 0x00FF);
}

void Instructions::POP(u16 *reg) {
    *reg = (m_mem->MemR(m_reg->GetSP() + 1) << 8) | m_mem->MemR(m_reg->GetSP());
	m_reg->AddSP(2);
}

void Instructions::LD_HL_cNN() {
    u16 address = _16bitsInmValue;
    m_reg->SetL(m_mem->MemR(address));
    m_reg->SetH(m_mem->MemR(address+1));
}

void Instructions::ADD(u16 *reg, u16 value) {
	m_reg->SetFlagN(0);
	m_reg->SetFlagH((((*reg & 0x0FFF) + (value & 0x0FFF)) > 0x0FFF) ? 1 : 0);
	m_reg->SetFlagC(((*reg + value) > 0xFFFF) ? 1 : 0);
	
    *reg += value;
}

void Instructions::LD(u8 *reg, u8 value)
{
    *reg = value;
}

void Instructions::LD(u16 *reg, u16 value)
{
    *reg = value;
}

void Instructions::CPI() {
    u8 hl = m_mem->MemR(m_reg->GetHL());
    u8 result = m_reg->GetA() - hl;
    
    m_reg->SetFlagS(BIT7(result) >> 7);
    m_reg->SetFlagZ(result == 0);
    m_reg->SetFlagH((m_reg->GetA() & 0x0F) < (hl & 0x0F));
    m_reg->SetFlagPV(m_reg->GetBC() != 0);
    m_reg->SetFlagN(1);
    
    m_reg->SetHL(m_reg->GetHL()+1);
    m_reg->SetBC(m_reg->GetBC()-1);
}

void Instructions::NEG() {
    u8 a = m_reg->GetA();
    u8 result = 0 - a;
    
    m_reg->SetFlagS(BIT7(result) >> 7);
    m_reg->SetFlagZ(result == 0);
    m_reg->SetFlagH((a & 0x0F) != 0);
    m_reg->SetFlagPV(a == 0x80);
    m_reg->SetFlagN(1);
    m_reg->SetFlagC(a != 0);
    
    m_reg->SetA(result);
}

void Instructions::LD_Mem(u16 address, u8 value) {
    m_mem->MemW(address, value);
}

void Instructions::LD_Mem(u16 address, u16 value) {
    m_mem->MemW(address+0, value & 0x00FF);
    m_mem->MemW(address+1, value >> 8);
}

void Instructions::LD_Content(u16 *reg, u16 address) {
    u8 valueL = m_mem->MemR(address + 0);
    u8 valueH = m_mem->MemR(address + 1);
    
    *reg = (valueH << 8) | valueL;
}

void Instructions::BIT(u8 bit, u8 value) {
    value &= (1 << bit);
    
	m_reg->SetFlagZ(value ? 0 : 1);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(1);
}

void Instructions::SET_Mem(u8 bit, u16 address) {
    u8 value = m_mem->MemR(address);
    value |= (1 << bit);
    m_mem->MemW(address, value);
}

void Instructions::RES_Mem(u8 bit, u16 address) {
    u8 value = m_mem->MemR(address);
    value &= ~(1 << bit);
    m_mem->MemW(address, value);
}

void Instructions::LDD() {
    u8 value = m_mem->MemR(m_reg->GetHL());
    m_mem->MemW(m_reg->GetDE(), value);
    
    m_reg->SetBC(m_reg->GetBC()-1);
    m_reg->SetDE(m_reg->GetDE()-1);
    m_reg->SetHL(m_reg->GetHL()-1);
    
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(m_reg->GetBC() ? 1 : 0);
    m_reg->SetFlagN(0);
}

void Instructions::LDDR() {
    LDD();
    if(m_reg->GetBC()) // Si se cumple la condición evitar que salte
        m_reg->SetIncPC(false);
}

void Instructions::OR(u8 value)
{
	u8 a = m_reg->GetA();
    value |= a;
    
	m_reg->SetA(value);
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
	m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
    m_reg->SetFlagN(0);
	m_reg->SetFlagC(0);
}

void Instructions::ADD(u8 value)
{
    u8 a = m_reg->GetA();
	u8 result = a + value;
    
    m_reg->SetFlagS(result >> 7);
	m_reg->SetFlagZ(result ? 0 : 1);
    m_reg->SetFlagH((((a & 0x0F) + (value & 0x0F)) > 0x0F) ? 1 : 0);
    m_reg->SetFlagPV((result < a) ? 1 : 0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagC((result < a) ? 1 : 0);
    
	m_reg->SetA(result);
}

void Instructions::INC_NoFlags(u16 *reg) {
    *reg = *reg+1;
}

void Instructions::INC(u8 *reg) {
    u8 oldValue = *reg;
    u8 newValue = oldValue + 1;
    *reg = newValue;
    
    m_reg->SetFlagS(newValue >> 7);
	m_reg->SetFlagZ(newValue ? 0 : 1);
    m_reg->SetFlagH((newValue & 0x0F) ? 0 : 1);
    m_reg->SetFlagPV((oldValue == 0x7F) ? 1 : 0);
	m_reg->SetFlagN(0);
}

void Instructions::INC_Mem(u16 address) {
    u8 oldValue = m_mem->MemR(address);
    u8 newValue = oldValue + 1;
    m_mem->MemW(address, newValue);
    
    m_reg->SetFlagS(newValue >> 7);
	m_reg->SetFlagZ(newValue ? 0 : 1);
    m_reg->SetFlagH((newValue & 0x0F) ? 0 : 1);
    m_reg->SetFlagPV((oldValue == 0x7F) ? 1 : 0);
	m_reg->SetFlagN(0);
}

void Instructions::DEC_NoFlags(u16 *reg) {
    *reg = *reg-1;
}

void Instructions::DEC(u8 *reg) {
    u8 oldValue = *reg;
    u8 newValue = oldValue - 1;
    *reg = newValue;
	
    m_reg->SetFlagS(newValue >> 7);
	m_reg->SetFlagZ(newValue ? 0 : 1);
    m_reg->SetFlagH((newValue & 0x0F) == 0x0F ? 1 : 0);
    m_reg->SetFlagPV((oldValue == 0x80) ? 1 : 0);
	m_reg->SetFlagN(1);
}

void Instructions::DEC_Mem(u16 address) {
    u8 oldValue = m_mem->MemR(address);
    u8 newValue = oldValue - 1;
    m_mem->MemW(address, newValue);
	
    m_reg->SetFlagS(newValue >> 7);
	m_reg->SetFlagZ(newValue ? 0 : 1);
    m_reg->SetFlagH((newValue & 0x0F) == 0x0F ? 1 : 0);
    m_reg->SetFlagPV((oldValue == 0x80) ? 1 : 0);
	m_reg->SetFlagN(1);
}

void Instructions::ADC(u8 value)
{
	u8 result = m_reg->GetA() + value + m_reg->GetFlagC();
    
    m_reg->SetFlagS(result >> 7);
	m_reg->SetFlagZ(result ? 0 : 1);
	if ((result & 0x0F) < (m_reg->GetA() & 0x0F))
		m_reg->SetFlagH(1);
	else
		m_reg->SetFlagH(0);
    m_reg->SetFlagPV((result < m_reg->GetA()) ? 1 : 0);
    m_reg->SetFlagN(0);
	m_reg->SetFlagC((result < m_reg->GetA()) ? 1 : 0);
    
	m_reg->SetA(result);
}

void Instructions::ADC(u16 value) {
    u16 result = m_reg->GetHL() + value + m_reg->GetFlagC();
    
    m_reg->SetFlagS(result >> 15);
	m_reg->SetFlagZ(result ? 0 : 1);
	if ((result & 0x0FFF) < (m_reg->GetHL() & 0x0FFF))
		m_reg->SetFlagH(1);
	else
		m_reg->SetFlagH(0);
    m_reg->SetFlagPV((result < m_reg->GetHL()) ? 1 : 0);
    m_reg->SetFlagN(0);
	m_reg->SetFlagC((result < m_reg->GetHL()) ? 1 : 0);
    
	m_reg->SetHL(result);
}

void Instructions::CP(u8 value)
{
    u8 result = m_reg->GetA() - value;
    
    m_reg->SetFlagS(result >> 7);
	m_reg->SetFlagZ(result ? 0 : 1);
    m_reg->SetFlagH(((value & 0x0F) > (m_reg->GetA() & 0x0F)) ? 1 : 0);
    m_reg->SetFlagPV((value > m_reg->GetA()) ? 1 : 0);
	m_reg->SetFlagN(1);
	m_reg->SetFlagC((value > m_reg->GetA()) ? 1 : 0);
}

void Instructions::SUB(u8 value)
{
	u8 result = m_reg->GetA() - value;
    
	m_reg->SetFlagS(result >> 7);
	m_reg->SetFlagZ(result ? 0 : 1);
    m_reg->SetFlagH(((value & 0x0F) > (m_reg->GetA() & 0x0F)) ? 1 : 0);
    m_reg->SetFlagPV((value > m_reg->GetA()) ? 1 : 0);
	m_reg->SetFlagN(1);
	m_reg->SetFlagC((value > m_reg->GetA()) ? 1 : 0);
    
	m_reg->SetA(result);
}

void Instructions::OUTD() {
    u8 value = m_mem->MemR(m_reg->GetHL());
    m_reg->SetHL(m_reg->GetHL()-1);
    m_reg->SetB(m_reg->GetB()-1);
    u8 port = m_reg->GetC();
    
    m_mem->PortW(port, value);
    
    m_reg->SetFlagZ(m_reg->GetB() ? 0 : 1);
    m_reg->SetFlagN(1);
}

void Instructions::OTDR() {
    
    OUTD();
    if(m_reg->GetB()) // Si se cumple la condición evitar que salte
        m_reg->SetIncPC(false);
}

void Instructions::RLCA() {
    u8 bit7 = BIT7(m_reg->GetA()) >> 7;
    m_reg->SetA((m_reg->GetA() << 1) | bit7);
    
    m_reg->SetFlagH(0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::RLC(u8 *reg) {
	u8 bit7 = BIT7(*reg) >> 7;
    *reg = ((*reg) << 1) | bit7;
    
    m_reg->SetFlagS(*reg >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::RLC_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit7 = BIT7(value) >> 7;
    value = (value << 1) | bit7;
    m_mem->MemW(address, value);
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::RLC_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit7 = BIT7(value) >> 7;
    value = (value << 1) | bit7;
    *reg = value;
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::AND(u8 value) {
    u8 result = m_reg->GetA() & value;
    m_reg->SetA(result);
    
    m_reg->SetFlagS(result >> 7);
	m_reg->SetFlagZ(result ? 0 : 1);
    m_reg->SetFlagH(1);
    m_reg->SetFlagPV(EvenBitsSet(result));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(0);
}

void Instructions::XOR(u8 value){
    u8 result = m_reg->GetA() ^ value;
    m_reg->SetA(result);
    
	m_reg->SetFlagS(result >> 7);
	m_reg->SetFlagZ(result ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(result));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(0);
}

void Instructions::RRCA() {
    u8 bit0 = BIT0(m_reg->GetA());
    m_reg->SetA((bit0 << 7) | (m_reg->GetA() >> 1));
    
    m_reg->SetFlagH(0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::RRC(u8 *reg) {
	u8 bit0 = BIT0(*reg);
    *reg = (bit0 << 7) | ((*reg) >> 1);
    
    m_reg->SetFlagS(*reg >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::RRC_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit0 = BIT0(value);
    value = (bit0 << 7) | (value >> 1);
    m_mem->MemW(address, value);
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::RRC_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit0 = BIT0(value);
    value = (bit0 << 7) | (value >> 1);
    *reg = value;
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::JP(u16 value) {
	m_reg->SetPC(value);
    m_reg->SetIncPC(false);
}

void Instructions::SBC(u8 value) {
    u8 result, sum;
    
	sum = value + m_reg->GetFlagC();
    result = m_reg->GetA() - sum;
    
    m_reg->SetFlagS(result >> 7);
	m_reg->SetFlagZ(result ? 0 : 1);
    m_reg->SetFlagH(((sum&0x0F) > (m_reg->GetA()&0x0F)) ? 1 : 0);
    m_reg->SetFlagPV((sum > m_reg->GetA()) ? 1 : 0);
	m_reg->SetFlagN(1);
    m_reg->SetFlagC((sum > m_reg->GetA()) ? 1 : 0);
    
	m_reg->SetA(result);
}

void Instructions::SBC(u16 value) {
    u16 result, sum;
    
	sum = value + m_reg->GetFlagC();
    result = m_reg->GetHL() - sum;
    
    m_reg->SetFlagS(result >> 15);
	m_reg->SetFlagZ(result ? 0 : 1);
    m_reg->SetFlagH(((sum&0x0FFF) > (m_reg->GetHL()&0x0FFF)) ? 1 : 0);
    m_reg->SetFlagPV((sum > m_reg->GetHL()) ? 1 : 0);
	m_reg->SetFlagN(1);
    m_reg->SetFlagC((sum > m_reg->GetHL()) ? 1 : 0);
    
	m_reg->SetHL(result);
}

void Instructions::RLD() {
    u8 memValue = m_mem->MemR(m_reg->GetHL());
    u8 a = m_reg->GetA();
    
    m_mem->MemW(m_reg->GetHL(), (memValue << 4) | (a & 0x0F));
    m_reg->SetA((a & 0xF0) | (memValue >> 4));
    
    m_reg->SetFlagS(m_reg->GetA() >> 7);
    m_reg->SetFlagZ(m_reg->GetA() ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(m_reg->GetA()));
    m_reg->SetFlagN(0);
}

void Instructions::RLA() {
    u8 bit7 = BIT7(m_reg->GetA()) >> 7;
    m_reg->SetA((m_reg->GetA() << 1) | m_reg->GetFlagC());
    
    m_reg->SetFlagH(0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::RL(u8 *reg) {
	u8 bit7 = BIT7(*reg) >> 7;
    *reg = ((*reg) << 1) | m_reg->GetFlagC();
    
    m_reg->SetFlagS(*reg >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::RL_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit7 = BIT7(value) >> 7;
    value = (value << 1) | m_reg->GetFlagC();
    m_mem->MemW(address, value);
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::RL_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit7 = BIT7(value) >> 7;
    value = (value << 1) | m_reg->GetFlagC();
    *reg = value;
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::RRA() {
    u8 bit0 = BIT0(m_reg->GetA());
    m_reg->SetA((m_reg->GetFlagC() << 7) | (m_reg->GetA() >> 1));
    
    m_reg->SetFlagH(0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::RR(u8 *reg) {
	u8 bit0 = BIT0(*reg);
    *reg = (m_reg->GetFlagC() << 7) | ((*reg) >> 1);
    
    m_reg->SetFlagS(*reg >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::RR_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit0 = BIT0(value);
    value = (m_reg->GetFlagC() << 7) | (value >> 1);
    m_mem->MemW(address, value);
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::RR_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit0 = BIT0(value);
    value = (m_reg->GetFlagC() << 7) | (value >> 1);
    *reg = value;
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::SLA(u8 *reg) {
    u8 bit7 = BIT7(*reg);
    *reg = (*reg) << 1;
    
    m_reg->SetFlagS(*reg >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::SLA_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit7 = BIT7(value);
    value = value << 1;
    m_mem->MemW(address, value);
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::SLA_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit7 = BIT7(value);
    value = value << 1;
    *reg = value;
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::SRA(u8 *reg) {
	u8 bit0 = BIT0(*reg);
    u8 bit7 = BIT7(*reg);
    *reg = bit7 | ((*reg) >> 1);
    
    m_reg->SetFlagS(*reg >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::SRA_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit0 = BIT0(value);
    u8 bit7 = BIT7(value);
    value = bit7 | (value >> 1);
    m_mem->MemW(address, value);
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::SRA_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit0 = BIT0(value);
    u8 bit7 = BIT7(value);
    value = bit7 | (value >> 1);
    *reg = value;
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::SRL(u8 *reg) {
	u8 bit0 = BIT0(*reg);
    *reg = (*reg) >> 1;
    
    m_reg->SetFlagS(*reg >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::SRL_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit0 = BIT0(value);
    value = value >> 1;
    m_mem->MemW(address, value);
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::SRL_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit0 = BIT0(value);
    value = value >> 1;
    *reg = value;
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::SLL(u8 *reg) {
	u8 bit7 = BIT7(*reg);
    *reg = ((*reg) << 1) | 1;
    
    m_reg->SetFlagS(*reg >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::SLL_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit7 = BIT7(value);
    value = (value << 1) | 1;
    m_mem->MemW(address, value);
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::SLL_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    u8 bit7 = BIT7(value);
    value = (value << 1) | 1;
    *reg = value;
    
    m_reg->SetFlagS(value >> 7);
	m_reg->SetFlagZ(value ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::INI() {
    u8 value = m_mem->PortR(m_reg->GetC());
    m_mem->MemW(m_reg->GetHL(), value);
    m_reg->SetHL(m_reg->GetHL()+1);
    m_reg->SetB(m_reg->GetB()-1);
    
    m_reg->SetFlagZ(m_reg->GetB() ? 0 : 1);
    m_reg->SetFlagN(1);
}

void Instructions::INIR() {
    INI();
    if (m_reg->GetB())   // Si se cumple la condición evitar que salte
        m_reg->SetIncPC(false);
}

void Instructions::CPIR() {
    CPI();
    if (m_reg->GetBC())   // Si se cumple la condición evitar que salte
        m_reg->SetIncPC(false);
}

void Instructions::CPD() {
    u8 value = m_mem->MemR(m_reg->GetHL());
    u8 result = m_reg->GetA() - value;
    m_reg->SetHL(m_reg->GetHL()-1);
    m_reg->SetBC(m_reg->GetBC()-1);
    
    m_reg->SetFlagS(result >> 7);
	m_reg->SetFlagZ(result ? 0 : 1);
    m_reg->SetFlagH((result & 0x0F) > (value & 0x0F));
    m_reg->SetFlagPV(m_reg->GetBC() ? 0 : 1);
	m_reg->SetFlagN(1);
}

void Instructions::CPDR() {
    CPD();
    if (m_reg->GetBC() && (!m_reg->GetFlagZ()))   // Si se cumple la condición evitar que salte
        m_reg->SetIncPC(false);
}
