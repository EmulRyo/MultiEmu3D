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
}

Instructions::~Instructions(void)
{
}

void Instructions::NOP() { }

void Instructions::CPL() {
	m_reg->SetA(~m_reg->GetA());

	m_reg->SetFlagN(1);
	m_reg->SetFlagH(1);
}

void Instructions::JR() {
    char address = _8bitsInmValue; //Con signo

	//El "2 +" es porque antes de saltar ha tenido que ver cuales eran los dos opcodes de la instruccion
	//y tiene importancia al ser un salto relativo con respecto al actual PC.
    m_reg->AddPC(2 + address);
    m_reg->SetIncPC(false);
}

void Instructions::JR_CC_n(e_registers flag, u8 value2check) {
	if (m_reg->GetFlag(flag) == value2check) {
		JR();
        m_reg->SetConditionalTaken(true);
    }
}

void Instructions::CALL_nn() {
	m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(),((m_reg->GetPC() + 3) & 0xFF00) >> 8);
	m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(),(m_reg->GetPC() + 3) & 0x00FF);
	m_reg->SetPC(_16bitsInmValue);
    m_reg->SetIncPC(false);
}

void Instructions::CALL_cc_nn(e_registers flag, u8 value2check) {
	if (m_reg->GetFlag(flag) == value2check) {
		CALL_nn();
        m_reg->SetConditionalTaken(true);
    }
}

void Instructions::DAA() {
	u8 a = m_reg->GetA();
    u8 lowNibble = a & 0x0F;
    u8 newC = m_reg->GetFlagC();
    u8 newH = 0;
    
    // La operación anterior fue de suma
    if (m_reg->GetFlagN() == 0) {
        newC = 0;
        newH = (lowNibble > 9) ? 1 : 0;
        if (m_reg->GetFlagH() || (lowNibble > 9))
            a += 0x06;
        
        if (m_reg->GetFlagC() || (a > 0x9F)) {
            a += 0x60;
            newC = 1;
        }
    }
    else { // la operacion anterior fue de resta
        if (m_reg->GetFlagH()) {
            newH = (lowNibble < 6) ? 1 : 0;
            a = (a - 6) & 0xFF;
        }
        
        if (m_reg->GetFlagC())
            a -= 0x60;
    }
    
    m_reg->SetFlagS(a >> 7);
    m_reg->SetFlagZ(a ? 0 : 1);
    m_reg->SetFlagH(newH);
    m_reg->SetFlagPV(EvenBitsSet(a));
    m_reg->SetFlagC(newC);
    
    m_reg->SetA(a);
}

void Instructions::RET() {
	m_reg->SetPC((m_mem->MemR(m_reg->GetSP() + 1) << 8) | m_mem->MemR(m_reg->GetSP()));
	m_reg->AddSP(2);
    m_reg->SetIncPC(false);
}

void Instructions::RETI() {
	EI();
	RET();
}

void Instructions::RET_cc(e_registers flag, u8 value2check) {
	if (m_reg->GetFlag(flag) == value2check) {
		RET();
        m_reg->SetConditionalTaken(true);
    }
}

void Instructions::SET_b_r(u8 bit, e_registers place) {
	if (place == c_HL)
		m_mem->MemW(m_reg->GetHL(), m_mem->MemR(m_reg->GetHL()) | (1 << bit));
	else
		m_reg->SetReg(place, m_reg->GetReg(place) | (1 << bit));
}

void Instructions::RES_b_r(u8 bit, e_registers place) {
    if (place == c_HL)
		m_mem->MemW(m_reg->GetHL(), m_mem->MemR(m_reg->GetHL()) & ~(1 << bit));
	else
		m_reg->SetReg(place, m_reg->GetReg(place) & ~(1 << bit));
}

void Instructions::DI() {
	m_reg->SetIFF1(false);
    m_reg->SetIFF2(false);
}

void Instructions::EI() {
	m_reg->SetIntPending(true);
}

void Instructions::SCF() {
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(1);
}

void Instructions::HALT() {
	m_reg->SetHalt(true);
}

void Instructions::JP_cc_nn(e_registers flag, u8 value2check) {
	if (m_reg->GetFlag(flag) == value2check) {
        u16 nn = _16bitsInmValue;
		JP(nn);
    }
}

void Instructions::RST_n(u8 desp) {
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
    else
        m_reg->SetConditionalTaken(true);
}

void Instructions::OUT(u8 port, u8 value) {
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
    else
        m_reg->SetConditionalTaken(true);
}

u8 Instructions::EvenBitsSet(u8 v) {
    u8 c; // c accumulates the total bits set in v
    for (c = 0; v; c++)
        v &= v - 1; // clear the least significant bit set
    
    return ((c % 2) == 0);
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

void Instructions::IN(u8 *reg) {
    u8 port = m_reg->GetC();
    u8 value = m_mem->PortR(port);
    *reg = value;
    
    m_reg->SetFlagS(value >> 7);
    m_reg->SetFlagZ(!value);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(value));
    m_reg->SetFlagN(0);
}

void Instructions::IN_OnlyFlags() {
    u8 tmp = 0;
    IN(&tmp);
}

void Instructions::LD_A_I() {
    u8 i = m_reg->GetI();
    m_reg->SetA(i);
    
    m_reg->SetFlagS(i >> 7);
    m_reg->SetFlagZ(i ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(m_reg->GetIFF2());
    m_reg->SetFlagN(0);
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

void Instructions::PUSH(u16 value) {
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

void Instructions::LD(u8 *reg, u8 value) {
    *reg = value;
}

void Instructions::LD(u16 *reg, u16 value) {
    *reg = value;
}

void Instructions::CPI() {
    u8 hl = m_mem->MemR(m_reg->GetHL());
    u8 result = m_reg->GetA() - hl;
    
    m_reg->SetHL(m_reg->GetHL()+1);
    m_reg->SetBC(m_reg->GetBC()-1);
    
    m_reg->SetFlagS(result >> 7);
    m_reg->SetFlagZ(result ? 0 : 1);
    m_reg->SetFlagH((result & 0x0F) > (m_reg->GetA() & 0x0F) ? 1 : 0);
    m_reg->SetFlagPV(m_reg->GetBC() ? 1 : 0);
    m_reg->SetFlagN(1);
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
	m_reg->SetFlagH(1);
    m_reg->SetFlagPV(m_reg->GetFlagZ());
    m_reg->SetFlagN(0);
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
    else
        m_reg->SetConditionalTaken(true);
}

void Instructions::OR(u8 value) {
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

void Instructions::ADD(u8 value) {
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

void Instructions::ADC(u8 value) {
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

void Instructions::CP(u8 value) {
    u8 result = m_reg->GetA() - value;
    
    m_reg->SetFlagS(result >> 7);
	m_reg->SetFlagZ(result ? 0 : 1);
    m_reg->SetFlagH(((value & 0x0F) > (m_reg->GetA() & 0x0F)) ? 1 : 0);
    m_reg->SetFlagPV((value > m_reg->GetA()) ? 1 : 0);
	m_reg->SetFlagN(1);
	m_reg->SetFlagC((value > m_reg->GetA()) ? 1 : 0);
}

void Instructions::SUB(u8 value) {
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
    if(m_reg->GetB()) { // Si se cumple la condición evitar que salte
        m_reg->SetIncPC(false);
        m_reg->SetConditionalTaken(true);
    }
}

void Instructions::RLCA() {
    u8 bit7 = m_reg->GetA() >> 7;
    m_reg->SetA((m_reg->GetA() << 1) | bit7);
    
    m_reg->SetFlagH(0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::RLC(u8 *reg) {
	u8 bit7 = (*reg) >> 7;
    *reg = ((*reg) << 1) | bit7;
    
    m_reg->SetFlagS((*reg) >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::RLC_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    RLC(&value);
    m_mem->MemW(address, value);
}

void Instructions::RLC_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    RLC(&value);
    *reg = value;
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
    
    m_reg->SetFlagS((*reg) >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::RRC_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    RRC(&value);
    m_mem->MemW(address, value);
}

void Instructions::RRC_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    RRC(&value);
    *reg = value;
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

void Instructions::RRD() {
    u8 memValue = m_mem->MemR(m_reg->GetHL());
    u8 a = m_reg->GetA();
    
    m_mem->MemW(m_reg->GetHL(), (a << 4) | (memValue >> 4));
    m_reg->SetA((a & 0xF0) | (memValue & 0x0F));
    
    m_reg->SetFlagS(m_reg->GetA() >> 7);
    m_reg->SetFlagZ(m_reg->GetA() ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(m_reg->GetA()));
    m_reg->SetFlagN(0);
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
    u8 bit7 = m_reg->GetA() >> 7;
    m_reg->SetA((m_reg->GetA() << 1) | m_reg->GetFlagC());
    
    m_reg->SetFlagH(0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::RL(u8 *reg) {
	u8 bit7 = (*reg) >> 7;
    *reg = ((*reg) << 1) | m_reg->GetFlagC();
    
    m_reg->SetFlagS((*reg) >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::RL_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    RL(&value);
    m_mem->MemW(address, value);
}

void Instructions::RL_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    RL(&value);
    *reg = value;
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
    
    m_reg->SetFlagS((*reg) >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::RR_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    RR(&value);
    m_mem->MemW(address, value);
}

void Instructions::RR_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    RR(&value);
    *reg = value;
}

void Instructions::SLA(u8 *reg) {
    u8 bit7 = (*reg) >> 7;
    *reg = (*reg) << 1;
    
    m_reg->SetFlagS((*reg) >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::SLA_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    SLA(&value);
    m_mem->MemW(address, value);
}

void Instructions::SLA_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    SLA(&value);
    *reg = value;
}

void Instructions::SRA(u8 *reg) {
	u8 bit0 = BIT0(*reg);
    u8 bit7 = BIT7(*reg);
    *reg = bit7 | ((*reg) >> 1);
    
    m_reg->SetFlagS((*reg) >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::SRA_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    SRA(&value);
    m_mem->MemW(address, value);
}

void Instructions::SRA_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    SRA(&value);
    *reg = value;
}

void Instructions::SRL(u8 *reg) {
	u8 bit0 = BIT0(*reg);
    *reg = (*reg) >> 1;
    
    m_reg->SetFlagS((*reg) >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit0);
}

void Instructions::SRL_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    SRL(&value);
    m_mem->MemW(address, value);
}

void Instructions::SRL_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    SRL(&value);
    *reg = value;
}

void Instructions::SLL(u8 *reg) {
	u8 bit7 = (*reg) >> 7;
    *reg = ((*reg) << 1) | 1;
    
    m_reg->SetFlagS((*reg) >> 7);
	m_reg->SetFlagZ((*reg) ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(EvenBitsSet(*reg));
	m_reg->SetFlagN(0);
	m_reg->SetFlagC(bit7);
}

void Instructions::SLL_Mem(u16 address) {
    u8 value = m_mem->MemR(address);
    SLL(&value);
    m_mem->MemW(address, value);
}

void Instructions::SLL_Mem(u8 *reg, u16 address) {
    u8 value = m_mem->MemR(address);
    SLL(&value);
    *reg = value;
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
    if (m_reg->GetB())  // Si se cumple la condición evitar que salte
        m_reg->SetIncPC(false);
    else
        m_reg->SetConditionalTaken(true);
}

void Instructions::CPIR() {
    CPI();
    if (m_reg->GetBC() && (!m_reg->GetFlagZ())) // Si se cumple la condición evitar que salte
        m_reg->SetIncPC(false);
    else
        m_reg->SetConditionalTaken(true);
}

void Instructions::CPD() {
    u8 value = m_mem->MemR(m_reg->GetHL());
    u8 result = m_reg->GetA() - value;
    m_reg->SetHL(m_reg->GetHL()-1);
    m_reg->SetBC(m_reg->GetBC()-1);
    
    m_reg->SetFlagS(result >> 7);
	m_reg->SetFlagZ(result ? 0 : 1);
    m_reg->SetFlagH((result & 0x0F) > (m_reg->GetA() & 0x0F));
    m_reg->SetFlagPV(m_reg->GetBC() ? 1 : 0);
	m_reg->SetFlagN(1);
}

void Instructions::CPDR() {
    CPD();
    if (m_reg->GetBC() && (!m_reg->GetFlagZ())) // Si se cumple la condición evitar que salte
        m_reg->SetIncPC(false);
    else
        m_reg->SetConditionalTaken(true);
}

void Instructions::RETN() {
    RET();
    m_reg->SetIFF1(m_reg->GetIFF2());
}