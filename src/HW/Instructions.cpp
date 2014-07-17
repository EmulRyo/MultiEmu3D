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

void Instructions::SetOpcode(u8 opcode) {
    m_opcode = opcode;
    
    if (opcode == 0xDD) {
        u8 nextOpcode = m_mem->MemR(m_reg->GetPC() + 1);
            
            switch (nextOpcode)
            {
                case 0x09: break;
                
                case 0x19: break;
                    
                case 0x21: break;
                case 0x29: break;
                    
                case 0x39: break;
                    
                case 0xE5: break;
                    
                default:
                    printf("Error, instruction not implemented: 0xDD%X\n", nextOpcode);
            }
    }
}

void Instructions::NOP(){m_reg->AddPC(1);}

void Instructions::LD_r1_r2(e_registers e_reg1, e_registers e_reg2)
{
	u8 length = 1;

	if (e_reg1 == c_HL)
	{
		if (e_reg2 == $)
		{
			m_mem->MemW(m_reg->GetHL(), _8bitsInmValue);
			length = 2;
		}
		else
			m_mem->MemW(m_reg->GetHL(), (u8)m_reg->GetReg(e_reg2));
	}
	else
	{
		if (e_reg2 == c_HL)
		{
			m_reg->SetReg(e_reg1, m_mem->MemR(m_reg->GetHL()));
		}
		else
		{
			m_reg->SetReg(e_reg1, m_reg->GetReg(e_reg2));
		}
	}

	m_reg->AddPC(length);
}


void Instructions::LD_A_n(e_registers place)
{
    int address, value, length = 1;

	switch(place)
	{
		case $:
			value = _8bitsInmValue;
			length = 2;
			break;
		case c_$$:
			address = _16bitsInmValue;
			value = m_mem->MemR(address);
			length = 3;
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

    m_reg->AddPC(length);
}


void Instructions::LD_n_A(e_registers place)
{
    int address, length = 1;

	switch (place)
	{
		case c_$$:
			address = _16bitsInmValue;
			m_mem->MemW(address, m_reg->GetA());
			length = 3;
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

	m_reg->AddPC(length);
}

void Instructions::JP_nn()
{
	m_reg->SetPC(_16bitsInmValue);
}

void Instructions::LDH_A_n()
{
	m_reg->SetA(m_mem->MemR(0xFF00 + _8bitsInmValue));
	m_reg->AddPC(2);
}

void Instructions::LDH_c$_A()
{
	m_mem->MemW(0xFF00 + _8bitsInmValue, m_reg->GetA());
	m_reg->AddPC(2);
}

void Instructions::CP_n(e_registers place)
{
	u8 value;
	u8 length = 1;

	switch (place)
	{
		case $:
			value = _8bitsInmValue;
			length = 2;
			break;
		case c_HL:
			value = m_mem->MemR(m_reg->GetHL());
			break;
		default:
			value = (u8)m_reg->GetReg(place);
	}

	m_reg->SetFlagZ((m_reg->GetA() == value) ? 1 : 0);
	m_reg->SetFlagN(1);
	m_reg->SetFlagH(((m_reg->GetA() & 0x0F) < (value & 0x0F)) ? 1 : 0);
	m_reg->SetFlagC((m_reg->GetA() < value) ? 1 : 0);

	m_reg->AddPC(length);
}

void Instructions::CPL()
{
	m_reg->SetA(~m_reg->GetA());

	m_reg->SetFlagN(1);
	m_reg->SetFlagH(1);

	m_reg->AddPC(1);
}

void Instructions::LD_n_nn(e_registers place)
{
	assert((place == BC) || (place == DE) || (place == HL) || (place == SP) || (place == IX));
    if (m_opcode == 0xDD)
        place = IX;
	m_reg->SetReg(place, _16bitsInmValue);
    m_reg->AddPC(3);
}

void Instructions::JR()
{
    char address;	//Con signo

	address = _8bitsInmValue;

	//El "2 +" es porque antes de saltar ha tenido que ver cuales eran los dos opcodes de la instruccion
	//y tiene importancia al ser un salto relativo con respecto al actual PC.
    m_reg->AddPC(2 + address);
}

void Instructions::JR_CC_n(e_registers flag, u8 value2check)
{
	if (m_reg->GetFlag(flag) == value2check)
    {
		JR();
        m_reg->SetConditionalTaken(true);
    }
	else
		m_reg->AddPC(2);
}

void Instructions::CALL_nn()
{
	m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(),((m_reg->GetPC() + 3) & 0xFF00) >> 8);
	m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(),(m_reg->GetPC() + 3) & 0x00FF);
	m_reg->SetPC(_16bitsInmValue);
}

void Instructions::CALL_cc_nn(e_registers flag, u8 value2check)
{
	if (m_reg->GetFlag(flag) == value2check)
    {
		CALL_nn();
        m_reg->SetConditionalTaken(true);
    }
	else
		m_reg->AddPC(3);
}

void Instructions::LDI_A_cHL()
{
	m_reg->SetA(m_mem->MemR(m_reg->GetHL()));
	m_reg->SetHL(m_reg->GetHL() + 1);
	m_reg->AddPC(1);
}

void Instructions::LDI_cHL_A()
{
	m_mem->MemW(m_reg->GetHL(), m_reg->GetA());
	m_reg->SetHL(m_reg->GetHL() + 1);
	m_reg->AddPC(1);
}

void Instructions::LDD_A_cHL()
{
	m_reg->SetA(m_mem->MemR(m_reg->GetHL()));
	m_reg->SetHL(m_reg->GetHL() - 1);
	m_reg->AddPC(1);
}

void Instructions::LDD_cHL_A()
{
	m_mem->MemW(m_reg->GetHL(), m_reg->GetA());
	m_reg->SetHL(m_reg->GetHL() - 1);
	m_reg->AddPC(1);
}

void Instructions::LD_SP_HL()
{
	m_reg->SetSP(m_reg->GetHL());

	m_reg->AddPC(1);
}

void Instructions::SUB_n(e_registers place)
{
	int value, length = 1;

	if (place == c_HL)
		value = m_mem->MemR(m_reg->GetHL());
	else if (place == $)
	{
		value = _8bitsInmValue;
		length = 2;
	}
	else
		value = m_reg->GetReg(place);

	m_reg->SetFlagZ((m_reg->GetA() - value) ? 0 : 1);
	m_reg->SetFlagN(1);
	m_reg->SetFlagH(((m_reg->GetA() & 0x0F) < (value & 0x0F)) ? 1 : 0);
	m_reg->SetFlagC((m_reg->GetA() < value) ? 1 : 0);

	m_reg->SetA(m_reg->GetA() - value);

	m_reg->AddPC(length);
}

void Instructions::ADD_A_n(e_registers place)
{
	int value, length = 1;
	u8 valueReg;

	switch (place)
	{
		case $:		valueReg = _8bitsInmValue; length = 2; break;
		case c_HL:	valueReg = m_mem->MemR(m_reg->GetHL()); break;
		default:	valueReg = (u8)m_reg->GetReg(place); break;
	}

	value = m_reg->GetA() + valueReg;

	m_reg->SetFlagZ(!(value & 0xFF) ? 1 : 0);
	if (((m_reg->GetA() & 0x0F) + (valueReg & 0x0F)) > 0x0F) m_reg->SetFlagH(1); else m_reg->SetFlagH(0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagC((value > 0xFF) ? 1 : 0);

	m_reg->SetA(value & 0xFF);

	m_reg->AddPC(length);
}

void Instructions::ADC_A_n(e_registers place)
{
	int value, length = 1;
	u8 valueReg;

	switch (place)
	{
		case $:		valueReg = _8bitsInmValue; length = 2; break;
		case c_HL:	valueReg = m_mem->MemR(m_reg->GetHL()); break;
		default:	valueReg = (u8)m_reg->GetReg(place); break;
	}

	value = m_reg->GetFlagC() + valueReg + m_reg->GetA();

	m_reg->SetFlagZ(!(value & 0xFF) ? 1 : 0);
	m_reg->SetFlagN(0);
	if ((m_reg->GetFlagC() + (valueReg & 0x0F) + (m_reg->GetA() & 0x0F)) > 0x0F)
		m_reg->SetFlagH(1);
	else
		m_reg->SetFlagH(0);
	m_reg->SetFlagC((value > 0xFF) ? 1 : 0);

	m_reg->SetA(value & 0xFF);

	m_reg->AddPC(length);
}

void Instructions::INC_n(e_registers place)
{
	u8 value;

	if (place == c_HL)
	{
		value = m_mem->MemR(m_reg->GetHL()) + 1;
		m_mem->MemW(m_reg->GetHL(), value);
	}
	else
	{
		value = m_reg->GetReg(place) + 1;
		m_reg->SetReg(place, value);
	}
	m_reg->SetFlagZ(value ? 0 : 1);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH((value & 0x0F) ? 0 : 1);

	m_reg->AddPC(1);
}

void Instructions::ADD_HL_n(e_registers place)
{
	u16 value, hl;
	
	value = m_reg->GetReg(place);
    if (m_opcode == 0xDD) {
        hl = m_reg->GetIX();
        if (place == HL)
            place = IX;
    }
    else
        hl = m_reg->GetHL();
	
	m_reg->SetFlagN(0);
	m_reg->SetFlagH((((hl & 0x0FFF) + (value & 0x0FFF)) > 0x0FFF) ? 1 : 0);
	m_reg->SetFlagC(((hl + value) > 0xFFFF) ? 1 : 0);
	
    if (m_opcode == 0xDD)
        m_reg->SetIX(hl + value);
    else
        m_reg->SetHL(hl + value);
    
    m_reg->AddPC(1);
}

void Instructions::RLC_n(e_registers place)
{
	u8 bit7, value;

	if (place == c_HL)
	{
		value = m_mem->MemR(m_reg->GetHL());
		bit7 = BIT7(value) >> 7;
		value = (value << 1) | bit7;
		m_mem->MemW(m_reg->GetHL(), value);
	}
	else
	{
		value = (u8)m_reg->GetReg(place);
		bit7 = BIT7(value) >> 7;
		value = (value << 1) | bit7;
		m_reg->SetReg(place, value);
	}

	m_reg->SetFlagZ(0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(bit7);

    if (m_mem->MemR(m_reg->GetPC()) == 0xCB)
    {
        m_reg->SetFlagZ(value ? 0 : 1);
		m_reg->AddPC(2);
    }
	else
		m_reg->AddPC(1);
}

void Instructions::INC_nn(e_registers place)
{
	m_reg->SetReg(place, m_reg->GetReg(place) + 1);
	m_reg->AddPC(1);
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
    
    m_reg->AddPC(1);
}

void Instructions::DEC_n(e_registers place)
{
	u8 value;

	if (place == c_HL)
	{
		value = m_mem->MemR(m_reg->GetHL()) - 1;
		m_mem->MemW(m_reg->GetHL(), value);
	}
	else
	{
		value = m_reg->GetReg(place) - 1;
		m_reg->SetReg(place, value);
	}
	
	m_reg->SetFlagZ(!value ? 1 : 0);
	m_reg->SetFlagN(1);
	m_reg->SetFlagH(((value & 0x0F) == 0x0F) ? 1 : 0);

	m_reg->AddPC(1);
}

void Instructions::DEC_nn(e_registers place)
{
	m_reg->SetReg(place, m_reg->GetReg(place) - 1);
	m_reg->AddPC(1);
}

void Instructions::OR_n(e_registers place)
{
	u8 longitud = 1;
    u8 value;

	switch (place)
	{
		case $:
			value = m_reg->GetA() | _8bitsInmValue;
			longitud = 2;
			break;
		case c_HL:
			value = m_reg->GetA() | m_mem->MemR(m_reg->GetHL());
			break;
		default:
			value = m_reg->GetA() | m_reg->GetReg(place);
	}

    m_reg->SetA(value);

	m_reg->SetFlagZ(value ? 0 : 1);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(0);

	m_reg->AddPC(longitud);
}

void Instructions::XOR_n(e_registers place)
{
    u8 longitud = 1;
    u8 value;

	switch (place)
	{
		case $:
			value = m_reg->GetA() ^ _8bitsInmValue;
			longitud = 2;
			break;
		case c_HL:
			value = m_reg->GetA() ^ m_mem->MemR(m_reg->GetHL());
			break;
		default:
			value = m_reg->GetA() ^ m_reg->GetReg(place);
	}
    
    m_reg->SetA(value);

	m_reg->SetFlagZ(value ? 0 : 1);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(0);

	m_reg->AddPC(longitud);
}

void Instructions::RET()
{
	m_reg->SetPC((m_mem->MemR(m_reg->GetSP() + 1) << 8) | m_mem->MemR(m_reg->GetSP()));
	m_reg->AddSP(2);
}

void Instructions::RETI()
{
	EI();
	RET();
}

void Instructions::RET_cc(e_registers flag, u8 value2check)
{
	m_reg->AddPC(1);
	if (m_reg->GetFlag(flag) == value2check)
    {
		RET();
        m_reg->SetConditionalTaken(true);
    }
}

void Instructions::LD_nn_n(e_registers place)
{
	m_reg->SetReg(place, _8bitsInmValue);
	m_reg->AddPC(2);
}

void Instructions::LD_A_cC()
{
	m_reg->SetA(m_mem->MemR(0xFF00 + m_reg->GetC()));
	m_reg->AddPC(1);
}

void Instructions::LD_cC_A()
{
	m_mem->MemW(0xFF00 + m_reg->GetC(), m_reg->GetA());
	m_reg->AddPC(1);
}

void Instructions::SET_b_r(u8 bit, e_registers place)
{
	if (place == c_HL)
		m_mem->MemW(m_reg->GetHL(), m_mem->MemR(m_reg->GetHL()) | (1 << bit));
	else
		m_reg->SetReg(place, m_reg->GetReg(place) | (1 << bit));

	m_reg->AddPC(2);
}

void Instructions::BIT_b_r(u8 bit, e_registers place)
{
	u8 value;

	if (place == c_HL)
		value = m_mem->MemR(m_reg->GetHL());
	else
		value = (u8)m_reg->GetReg(place);

	if (!(value & (1 << bit)))
		m_reg->SetFlagZ(1);
	else
		m_reg->SetFlagZ(0);

	m_reg->SetFlagN(0);
	m_reg->SetFlagH(1);

	m_reg->AddPC(2);
}

void Instructions::RES_b_r(u8 bit, e_registers place)
{
    if (place == c_HL)
		m_mem->MemW(m_reg->GetHL(), m_mem->MemR(m_reg->GetHL()) & ~(1 << bit));
	else
		m_reg->SetReg(place, m_reg->GetReg(place) & ~(1 << bit));

	m_reg->AddPC(2);
}

void Instructions::DI()
{
	m_reg->SetIFF1(false);
    m_reg->SetIFF2(false);
	m_reg->AddPC(1);
}

void Instructions::EI()
{
	m_reg->SetIntPending(true);
	m_reg->AddPC(1);
}

void Instructions::SBC_A(e_registers place)
{
	u16 value;
    u8 result;
    int sum;
	int length = 1;

	switch(place)
	{
		case c_HL:
            value = m_mem->MemR(m_reg->GetHL());
            sum = value + m_reg->GetFlagC();
			break;
		case $:
            value = _8bitsInmValue;
            sum = value + m_reg->GetFlagC();
			length = 2;
			break;
		default:
            value = m_reg->GetReg(place);
			sum = value + m_reg->GetFlagC();
	}
    result = m_reg->GetA() - sum;
    
	m_reg->SetFlagZ(!result);
	m_reg->SetFlagN(1);
    
    if ((m_reg->GetA() & 0x0F) < (value & 0x0F))
        m_reg->SetFlagH(true);
    else if ((m_reg->GetA() & 0x0F) < (sum & 0x0F))
        m_reg->SetFlagH(true);
    else if (((m_reg->GetA() & 0x0F)==(value & 0x0F)) && ((value & 0x0F)==0x0F) && (m_reg->GetFlagC()))
        m_reg->SetFlagH(true);
    else
        m_reg->SetFlagH(false);
    
    m_reg->SetFlagC(m_reg->GetA() < sum);

	m_reg->SetA(result);

	m_reg->AddPC(length);
}

void Instructions::AND(e_registers place)
{
	u8 longitud = 1;

	switch (place)
	{
		case $:
			m_reg->SetA(m_reg->GetA() & _8bitsInmValue);
			longitud = 2;
			break;
		case c_HL:
			m_reg->SetA(m_reg->GetA() & m_mem->MemR(m_reg->GetHL()));
			break;
		default:
			m_reg->SetA(m_reg->GetA() & m_reg->GetReg(place));
	}

	m_reg->SetFlagZ(m_reg->GetA() ? 0 : 1);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(1);
	m_reg->SetFlagC(0);

	m_reg->AddPC(longitud);
}


void Instructions::SLA_n(e_registers place)
{
	u8 bit7, value;

	if (place == c_HL)
	{
		bit7 = BIT7(m_mem->MemR(m_reg->GetHL())) >> 7;
		value = m_mem->MemR(m_reg->GetHL()) << 1;
		m_mem->MemW(m_reg->GetHL(), value);
	}
	else
	{
		bit7 = BIT7(m_reg->GetReg(place)) >> 7;
		value = m_reg->GetReg(place) << 1;
		m_reg->SetReg(place, value);
	}

	m_reg->SetFlagZ(!value ? 1 : 0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(bit7);

	m_reg->AddPC(2);
}

void Instructions::SRA_n(e_registers place)
{
    u8 bit0, bit7, value;

	if (place == c_HL)
	{
		bit0 = BIT0(m_mem->MemR(m_reg->GetHL()));
		bit7 = BIT7(m_mem->MemR(m_reg->GetHL()));
		m_mem->MemW(m_reg->GetHL(), bit7 | (m_mem->MemR(m_reg->GetHL()) >> 1));
		value = m_mem->MemR(m_reg->GetHL());
	}
	else
	{
		bit0 = BIT0(m_reg->GetReg(place));
		bit7 = BIT7(m_reg->GetReg(place));
		m_reg->SetReg(place, bit7 | (m_reg->GetReg(place) >> 1));
		value = (u8)m_reg->GetReg(place);
	}

	m_reg->SetFlagZ(!value ? 1 : 0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(bit0);

	m_reg->AddPC(2);
}

void Instructions::SRL_n(e_registers place)
{
    u8 bit0, value;

	if (place == c_HL)
	{
		bit0 = BIT0(m_mem->MemR(m_reg->GetHL()));
		value = m_mem->MemR(m_reg->GetHL()) >> 1;
		m_mem->MemW(m_reg->GetHL(), value);
	}
	else
	{
		bit0 = BIT0(m_reg->GetReg(place));
		value = m_reg->GetReg(place) >> 1;
		m_reg->SetReg(place, value);
	}

	m_reg->SetFlagZ(!value ? 1 : 0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(bit0);

	m_reg->AddPC(2);
}

void Instructions::ADD_SP_n()
{
	char n = _8bitsInmValue;

	m_reg->SetFlagZ(0);
	m_reg->SetFlagN(0);

    m_reg->SetFlagH(((m_reg->GetSP() & 0x0F) + (n & 0x0F)) > 0x0F);
    m_reg->SetFlagC(((m_reg->GetSP() & 0xFF) + (n & 0xFF)) > 0xFF);

	m_reg->AddSP(n);

	m_reg->AddPC(2);
}

void Instructions::JP_HL()
{
	m_reg->SetPC(m_reg->GetHL());
}

void Instructions::SCF()
{
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(1);

	m_reg->AddPC(1);
}

void Instructions::HALT()
{
	m_reg->SetHalt(true);
	
	m_reg->AddPC(1);

}

void Instructions::STOP()
{
    m_reg->AddPC(2);
}

void Instructions::SWAP(e_registers place)
{
	u8 value;

	if (place == c_HL)
	{
		value = m_mem->MemR(m_reg->GetHL());
		value = ((value & 0x0F) << 4) | ((value & 0xF0) >> 4);
		m_mem->MemW(m_reg->GetHL(), value);
	}
	else
	{
		value = (u8)m_reg->GetReg(place);
		value = ((value & 0x0F) << 4) | ((value & 0xF0) >> 4);
		m_reg->SetReg(place, value);
	}

	m_reg->SetFlagZ(value ? 0 : 1);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(0);

	m_reg->AddPC(2);
}

void Instructions::PUSH_nn(e_registers place)
{
    if (m_opcode == 0xDD)
        place = IX;
    
	m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(), (m_reg->GetReg(place) & 0xFF00) >> 8);
	m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(), m_reg->GetReg(place) & 0x00FF);

	m_reg->AddPC(1);
}

void Instructions::POP_nn(e_registers place)
{
	m_reg->SetReg(place, (m_mem->MemR(m_reg->GetSP() + 1) << 8) | m_mem->MemR(m_reg->GetSP()));
	m_reg->AddSP(2);

	m_reg->AddPC(1);
}

void Instructions::JP_cc_nn(e_registers flag, u8 value2check)
{
	u16 nn;

	nn = _16bitsInmValue;

	m_reg->AddPC(3);

	if (m_reg->GetFlag(flag) == value2check)
    {
		m_reg->SetPC(nn);
        m_reg->SetConditionalTaken(true);
    }
}

void Instructions::RL_n(e_registers place)
{
	u8 oldBit7, value;

	if (place == c_HL)
	{
		oldBit7 = BIT7(m_mem->MemR(m_reg->GetHL())) >> 7;
		value = (m_mem->MemR(m_reg->GetHL()) << 1) | m_reg->GetFlagC();
		m_mem->MemW(m_reg->GetHL(), value);
	}
	else
	{
		oldBit7 = BIT7(m_reg->GetReg(place)) >> 7;
		value = (m_reg->GetReg(place) << 1) | m_reg->GetFlagC();
		m_reg->SetReg(place, value);
	}

	m_reg->SetFlagZ(0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(oldBit7);

	if (m_mem->MemR(m_reg->GetPC()) == 0xCB)
    {
        m_reg->SetFlagZ(value ? 0 : 1);
		m_reg->AddPC(2);
    }
	else
		m_reg->AddPC(1);
}

void Instructions::RR_n(e_registers place)
{
	u8 bit0, value;

	if (place == c_HL)
	{
		value = m_mem->MemR(m_reg->GetHL());
		bit0 = BIT0(value);
		value = (m_reg->GetFlagC() << 7) | (value >> 1);
		m_mem->MemW(m_reg->GetHL(), value);
	}
	else
	{
		value = (u8)m_reg->GetReg(place);
		bit0 = BIT0(value);
		value = (m_reg->GetFlagC() << 7) | (value >> 1);
		m_reg->SetReg(place, value);
	}

	m_reg->SetFlagZ(0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(bit0);

	if (m_mem->MemR(m_reg->GetPC()) == 0xCB)
    {
        m_reg->SetFlagZ(value ? 0 : 1);
		m_reg->AddPC(2);
    }
	else
		m_reg->AddPC(1);
}

void Instructions::RRC_n(e_registers place)
{
	u8 bit0, value;

	if (place == c_HL)
	{
		value = m_mem->MemR(m_reg->GetHL());
		bit0 = BIT0(value);
		value = (bit0 << 7) | (value >> 1);
		m_mem->MemW(m_reg->GetHL(), value);
	}
	else
	{
		value = (u8)m_reg->GetReg(place);
		bit0 = BIT0(value);
		value = (bit0 << 7) | (value >> 1);
		m_reg->SetReg(place, value);
	}

	m_reg->SetFlagZ(0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(bit0);

	if (m_mem->MemR(m_reg->GetPC()) == 0xCB)
    {
        m_reg->SetFlagZ(value ? 0 : 1);
		m_reg->AddPC(2);
    }
	else
		m_reg->AddPC(1);
}

void Instructions::PUSH_PC()
{
	m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(), (m_reg->GetPC() & 0xFF00) >> 8);
	m_reg->AddSP(-1);
	m_mem->MemW(m_reg->GetSP(), m_reg->GetPC() & 0x00FF);
}

void Instructions::RST_n(u8 desp)
{
	//Queremos que se guarde la siquiente instruccion a ejecutar
	m_reg->AddPC(1);

	PUSH_PC();

	m_reg->SetPC(0x0000 + desp);
}

void Instructions::LDHL_SP_n()
{
	char n = _8bitsInmValue;

	m_reg->SetFlagZ(0);
	m_reg->SetFlagN(0);
    m_reg->SetFlagH(((m_reg->GetSP() & 0x0F) + (n & 0x0F)) > 0x0F);
    m_reg->SetFlagC(((m_reg->GetSP() & 0xFF) + (n & 0xFF)) > 0xFF);

	m_reg->SetHL(m_reg->GetSP() + n);

	m_reg->AddPC(2);
}

/*
 *
 * Nuevas instruciones
 *
 */

void Instructions::IM(u8 mode) {
    m_reg->SetIntMode(mode);
    
    m_reg->AddPC(2);
}

void Instructions::DJNZ() {
    u8 value = m_reg->GetB() - 1;
    m_reg->SetB(value);
    if (value != 0) {
        JR();
        m_reg->SetConditionalTaken(true);
    } else {
        m_reg->AddPC(2);
    }
}

void Instructions::LD_cNN_nn(e_registers place) {
    assert((place == BC) || (place == DE) || (place == HL) || (place == SP));
    
    u16 address = _16bitsInmValue;
    u16 value = m_reg->GetReg(place);
    m_mem->MemW(address+0, (value & 0x00FF));
    m_mem->MemW(address+1, (value >> 8));
    
    if (m_opcode == 0xED)
        m_reg->AddPC(4);
    else
        m_reg->AddPC(3);
}

void Instructions::LD_cNN_n(e_registers place) {
    assert((place == A) || (place == B) || (place == C) ||
           (place == D) || (place == E) || (place == F));
    
    u16 address = _16bitsInmValue;
    u8  value = m_reg->GetReg(place);
    m_mem->MemW(address+0, value);
    
    m_reg->AddPC(3);
}

void Instructions::LDI(bool incrementPC) {
    u16 addressDE = m_reg->GetDE();
    u8 valueHL = m_mem->MemR(m_reg->GetHL());
    m_mem->MemW(addressDE, valueHL);
    m_reg->SetDE(addressDE+1);
    m_reg->SetHL(m_reg->GetHL()+1);
    m_reg->SetBC(m_reg->GetBC()-1);
    
    m_reg->SetFlagH(0);
    m_reg->SetFlagN(0);
    m_reg->SetFlagPV(!m_reg->GetBC() ? 0 : 1);
    
    if (incrementPC)
        m_reg->AddPC(2);
}

void Instructions::LDIR() {
    
    LDI(false);
    
    if (m_reg->GetBC() == 0) {
        m_reg->SetFlagH(0);
        m_reg->SetFlagN(0);
        m_reg->SetFlagPV(0);
        
        m_reg->AddPC(2);
    }
}

void Instructions::OUT(e_registers placePort, e_registers placeValue) {
    u8 port;
    
    if (placePort == $)
        port = _8bitsInmValue;
    else
        port = m_reg->GetReg(placePort);
	
    u8 value = (placeValue == f_Z) ? 0 : m_reg->GetReg(placeValue);
    
    m_mem->PortW(port, value);
    
    m_reg->AddPC(2);
}

void Instructions::OUTI(bool incrementPC) {
    u8 value = m_mem->MemR(m_reg->GetHL());
    m_reg->SetHL(m_reg->GetHL()+1);
    m_reg->SetB(m_reg->GetB()-1);
    u8 port = m_reg->GetC();
    
    m_mem->PortW(port, value);
    m_reg->SetFlagN(1);
    
    if (incrementPC)
        m_reg->AddPC(2);
}

void Instructions::OTIR() {
    
    OUTI(false);
    if(m_reg->GetB() == 0) {
        m_reg->SetFlagN(1);
        m_reg->SetFlagZ(1);
        m_reg->AddPC(2);
    }
}

u8 Instructions::EvenBitsSet(u8 v) {
    u8 c; // c accumulates the total bits set in v
    for (c = 0; v; c++)
        v &= v - 1; // clear the least significant bit set
    
    return (c % 2);
}

void Instructions::SLL_n(e_registers place)
{
	u8 bit7, value;
    
	if (place == c_HL)
	{
		bit7 = BIT7(m_mem->MemR(m_reg->GetHL())) >> 7;
		value = (m_mem->MemR(m_reg->GetHL()) << 1) | 1;
		m_mem->MemW(m_reg->GetHL(), value);
	}
	else
	{
		bit7 = BIT7(m_reg->GetReg(place)) >> 7;
		value = (m_reg->GetReg(place) << 1) | 1;
		m_reg->SetReg(place, value);
	}
    
	m_reg->SetFlagZ(!value ? 1 : 0);
	m_reg->SetFlagN(0);
	m_reg->SetFlagH(0);
	m_reg->SetFlagC(bit7);
    m_reg->SetFlagPV(EvenBitsSet(value));
    
	m_reg->AddPC(2);
}

void Instructions::CCF() {
    u8 c = m_reg->GetFlagC();
    m_reg->SetFlagH(c);
    m_reg->SetFlagN(0);
    m_reg->SetFlagC(c ? 0 : 1);
    
    m_reg->AddPC(1);
}

void Instructions::IN_A_n() {
    u8 port = _8bitsInmValue;
    u8 value = m_mem->PortR(port);
    m_reg->SetA(value);
    
    m_reg->AddPC(2);
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
    
    m_reg->AddPC(2);
}

void Instructions::SBC_HL(e_registers place) {
    u16 oldHL = m_reg->GetHL();
    u16 hl = oldHL - m_reg->GetReg(place) - m_reg->GetFlagC();
    m_reg->SetHL(hl);
    
    m_reg->SetFlagS(hl >> 15);
    m_reg->SetFlagZ(hl ? 0 : 1);
    if ((oldHL & 0x0FFF) < (hl & 0x0FFF))
        m_reg->SetFlagH(1);
    else
        m_reg->SetFlagH(0);
    m_reg->SetFlagPV(hl > oldHL ? 1 : 0);
    m_reg->SetFlagN(1);
    m_reg->SetFlagC(hl > oldHL ? 1 : 0);
    
     m_reg->AddPC(2);
}

void Instructions::LD_R_A() {
    m_reg->SetR(m_reg->GetA());
    
     m_reg->AddPC(2);
}

void Instructions::LD_A_R() {
    u8 r = m_reg->GetR();
    m_reg->SetA(r);
    
    m_reg->SetFlagS(r >> 7);
    m_reg->SetFlagZ(r ? 0 : 1);
    m_reg->SetFlagH(0);
    m_reg->SetFlagPV(m_reg->GetIFF2());
    m_reg->SetFlagN(0);
    
    m_reg->AddPC(2);
}

void Instructions::EX_DE_HL() {
    u16 de = m_reg->GetDE();
    u16 hl = m_reg->GetHL();
    
    m_reg->SetDE(hl);
    m_reg->SetHL(de);
    
    m_reg->AddPC(1);
}

void Instructions::EX_AF_AF2() {
    u16 af  = m_reg->GetAF();
    u16 af2 = m_reg->GetAF2();
    
    m_reg->SetAF(af2);
    m_reg->SetAF2(af);
    
    m_reg->AddPC(1);
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
    
    m_reg->AddPC(1);
}

void Instructions::EX_cSP_HL() {
    u8 h = m_reg->GetH();
    u8 l = m_reg->GetL();
    u8 sp1 = m_mem->MemR(m_reg->GetSP());
    u8 sp2 = m_mem->MemR(m_reg->GetSP()+1);
    
    m_reg->SetH(sp2);
    m_reg->SetL(sp1);
    m_mem->MemW(m_reg->GetSP(), l);
    m_mem->MemW(m_reg->GetSP()+1, h);
    
    m_reg->AddPC(1);
}

void Instructions::NOT_IMPLEMENTED() {
    printf("Not implemented\n");
}

