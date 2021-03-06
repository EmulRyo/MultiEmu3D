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

#include <iostream>
#include <assert.h>
#include "Registers.h"
#include "Memory.h"
#include "Instructions.h"
#include "../Common/Bit.h"

using namespace std;
using namespace Nes;

Instructions::Instructions(Registers* reg, Memory* mem) {
	m_reg = reg;
	m_mem = mem;
}

Instructions::~Instructions(void) {
    
}

u8 Instructions::Get8BitsInmValue() {
	return m_mem->MemR(m_reg->GetPC() + 1);
}

u16 Instructions::Get16BitsInmValue() {
	return ((m_mem->MemR(m_reg->GetPC() + 2)) << 8) | m_mem->MemR(m_reg->GetPC() + 1);
}

u8 Instructions::GetCyclesExtra() {
	return m_cyclesExtra;
}

void Instructions::ResetCyclesExtra() {
	m_cyclesExtra = 0;
}

void Instructions::ADC(u8 value, u8 length) {
	u8 result = m_reg->GetA() + value + m_reg->GetFlagC();
	u16 result16 = (u16)m_reg->GetA() + value + m_reg->GetFlagC();

	u8 c6 = ((m_reg->GetA() & 0x7F) + (value & 0x7F) + m_reg->GetFlagC()) >> 7;
	u8 c7 = result16 >> 8;

	m_reg->SetFlagC(result16 > 0xFF ? 1 : 0);
	m_reg->SetFlagV((c6 != c7) ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetA(result);
	m_reg->AddPC(length);

	if (m_mem->GetPageCrossed())
		m_cyclesExtra = 1;
}

void Instructions::SBC(u8 value, u8 length) {
	ADC(~value, length); // ~value = -value - 1
}

void Instructions::BMI() {
	if (m_reg->GetFlagN() == 1) {
		u16 oldAddress = m_reg->GetPC();
		s8 rel = (s8)Get8BitsInmValue();
		m_reg->AddPC(rel + 2);
		m_mem->PageCrossed(oldAddress, m_reg->GetPC());
		m_cyclesExtra = m_mem->GetPageCrossed() ? 2 : 1;
	}
	else
		m_reg->AddPC(2);
}

void Instructions::BPL() {
	if (m_reg->GetFlagN() == 0) {
		u16 oldAddress = m_reg->GetPC();
		s8 rel = (s8)Get8BitsInmValue();
		m_reg->AddPC(rel + 2);
		m_mem->PageCrossed(oldAddress, m_reg->GetPC());
		m_cyclesExtra = m_mem->GetPageCrossed() ? 2 : 1;
	}
	else
		m_reg->AddPC(2);
}

void Instructions::BCC() {
	if (m_reg->GetFlagC() == 0) {
		u16 oldAddress = m_reg->GetPC();
		s8 rel = (s8)Get8BitsInmValue();
		m_reg->AddPC(rel + 2);
		m_mem->PageCrossed(oldAddress, m_reg->GetPC());
		m_cyclesExtra = m_mem->GetPageCrossed() ? 2 : 1;
	}
	else
		m_reg->AddPC(2);
}

void Instructions::BCS() {
	if (m_reg->GetFlagC() == 1) {
		u16 oldAddress = m_reg->GetPC();
		s8 rel = (s8)Get8BitsInmValue();
		m_reg->AddPC(rel + 2);
		m_mem->PageCrossed(oldAddress, m_reg->GetPC());
		m_cyclesExtra = m_mem->GetPageCrossed() ? 2 : 1;
	}
	else
		m_reg->AddPC(2);
}
;
void Instructions::BEQ() {
	if (m_reg->GetFlagZ() == 1) {
		u16 oldAddress = m_reg->GetPC();
		s8 rel = (s8)Get8BitsInmValue();
		m_reg->AddPC(rel + 2);
		m_mem->PageCrossed(oldAddress, m_reg->GetPC());
		m_cyclesExtra = m_mem->GetPageCrossed() ? 2 : 1;
	}
	else
		m_reg->AddPC(2);
}

void Instructions::BNE() {
	if (m_reg->GetFlagZ() == 0) {
		u16 oldAddress = m_reg->GetPC();
		s8 rel = (s8)Get8BitsInmValue();
		m_reg->AddPC(rel + 2);
		m_mem->PageCrossed(oldAddress, m_reg->GetPC());
		m_cyclesExtra = m_mem->GetPageCrossed() ? 2 : 1;
	}
	else
		m_reg->AddPC(2);
}

void Instructions::BVS() {
	if (m_reg->GetFlagV() == 1) {
		u16 oldAddress = m_reg->GetPC();
		s8 rel = (s8)Get8BitsInmValue();
		m_reg->AddPC(rel + 2);
		m_mem->PageCrossed(oldAddress, m_reg->GetPC());
		m_cyclesExtra = m_mem->GetPageCrossed() ? 2 : 1;
	}
	else
		m_reg->AddPC(2);
}

void Instructions::BVC() {
	if (m_reg->GetFlagV() == 0) {
		u16 oldAddress = m_reg->GetPC();
		s8 rel = (s8)Get8BitsInmValue();
		m_reg->AddPC(rel + 2);
		m_mem->PageCrossed(oldAddress, m_reg->GetPC());
		m_cyclesExtra = m_mem->GetPageCrossed() ? 2 : 1;
	}
	else
		m_reg->AddPC(2);
}

void Instructions::CMP(u8 value, u8 length) {
	u8 result = m_reg->GetA() - value;
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->SetFlagC(m_reg->GetA() < value ? 0 : 1);
	m_reg->AddPC(length);

	if (m_mem->GetPageCrossed())
		m_cyclesExtra = 1;
}

void Instructions::CPX(u8 value, u8 length) {
	u8 result = m_reg->GetX() - value;
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->SetFlagC(m_reg->GetX() < value ? 0 : 1);
	m_reg->AddPC(length);
}

void Instructions::CPY(u8 value, u8 length) {
	u8 result = m_reg->GetY() - value;
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->SetFlagC(m_reg->GetY() < value ? 0 : 1);
	m_reg->AddPC(length);
}

void Instructions::JMP() {
	u16 address = Get16BitsInmValue();
	m_reg->SetPC(address);
}

void Instructions::JMPIndirect() {
	u16 indirectAddress = Get16BitsInmValue();
	u8 iaL = m_mem->MemR(indirectAddress);
	// La direcci�n del salto no puede estar en diferentes p�ginas
	if ((indirectAddress & 0xFF) == 0xFF)
		indirectAddress &= 0xFF00;
	else
		indirectAddress += 1;
	u8 iaH = m_mem->MemR(indirectAddress);
	u16 address = (iaH << 8) | iaL;
	m_reg->SetPC(address);
}

void Instructions::JSR() {
	u16 address = m_reg->GetPC() + 2;
	u8 pch = address >> 8;
	u8 pcl = address & 0xFF;

	u16 stackAddress = 0x100 | m_reg->GetS();
	m_mem->MemW(stackAddress, pch);
	m_mem->MemW(stackAddress-1, pcl);
	m_reg->SetS(m_reg->GetS() - 2);

	m_reg->SetPC(Get16BitsInmValue());
}

void Instructions::IRQ(u16 addressToStore, u8 stackBits45, u16 indAddressToJump) {
	u8 pch = addressToStore >> 8;
	u8 pcl = addressToStore & 0xFF;

	u8 regP = (m_reg->GetP() & 0xCF) | stackBits45; // Al guardarlo en el stack los bits 4 y 5 se fijan a un valor concreto

	u16 stackAddress = 0x100 | m_reg->GetS();
	m_mem->MemW(stackAddress, pch);
	m_mem->MemW(stackAddress - 1, pcl);
	m_mem->MemW(stackAddress - 2, regP);
	m_reg->SetS(m_reg->GetS() - 3);
	m_reg->SetFlagI(1);

	u16 newAddress = (m_mem->MemR(indAddressToJump +1) << 8) | m_mem->MemR(indAddressToJump);

	m_reg->SetPC(newAddress);
}

void Instructions::NMI() {
	IRQ(m_reg->GetPC(), 0x20, 0xFFFA);
}

void Instructions::BRK() {
	IRQ(m_reg->GetPC()+2, 0x30, 0xFFFE);
}

void Instructions::RTI() {
	u16 stackAddress = 0x100 | m_reg->GetS();
	m_reg->SetP(m_mem->MemR(stackAddress + 1));
	u8 pcl = m_mem->MemR(stackAddress + 2);
	u8 pch = m_mem->MemR(stackAddress + 3);
	m_reg->SetS(m_reg->GetS() + 3);

	u16 address = pch << 8 | pcl;
	m_reg->SetPC(address);
}

void Instructions::AND(u8 value, u8 length) {
	u8 result = m_reg->GetA() & value;
	m_reg->SetA(result);
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->AddPC(length);

	if (m_mem->GetPageCrossed())
		m_cyclesExtra = 1;
}

void Instructions::ORA(u8 value, u8 length) {
	u8 result = m_reg->GetA() | value;
	m_reg->SetA(result);
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->AddPC(length);

	if (m_mem->GetPageCrossed())
		m_cyclesExtra = 1;
}

void Instructions::EOR(u8 value, u8 length) {
	u8 result = m_reg->GetA() ^ value;
	m_reg->SetA(result);
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->AddPC(length);

	if (m_mem->GetPageCrossed())
		m_cyclesExtra = 1;
}

void Instructions::PHA() {
	u16 address = 0x100 | m_reg->GetS();
	m_mem->MemW(address, m_reg->GetA());
	m_reg->SetS(m_reg->GetS() - 1);
	m_reg->AddPC(1);
}

void Instructions::PHP() {
	u16 address = 0x100 | m_reg->GetS();
	u8 value = (m_reg->GetP() & 0xCF) | 0x30; // Al guardarlo en el stack los bits 4 y 5 se ponen a 11
	m_mem->MemW(address, value);
	m_reg->SetS(m_reg->GetS() - 1);
	m_reg->AddPC(1);
}

void Instructions::PLA() {
	u16 address = 0x100 | m_reg->GetS() + 1;
	u8 value = m_mem->MemR(address);
	m_reg->SetA(value);
	m_reg->SetS(address & 0xFF);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::PLP() {
	u16 address = 0x100 | m_reg->GetS()+1;
	m_reg->SetP(m_mem->MemR(address));
	m_reg->SetS(address & 0xFF);
	m_reg->AddPC(1);
}

void Instructions::RTS() {
	u16 stackAddress = 0x100 | m_reg->GetS();
	u8 pcl = m_mem->MemR(stackAddress + 1);
	u8 pch = m_mem->MemR(stackAddress + 2);
	m_reg->SetS(m_reg->GetS() + 2);

	u16 address = pch << 8 | pcl;
	m_reg->SetPC(address + 1);
}

void Instructions::SEI() {
	m_reg->SetFlagI(1);
	m_reg->AddPC(1);
}

void Instructions::CLI() {
	m_reg->SetFlagI(0);
	m_reg->AddPC(1);
}

void Instructions::SEC() {
	m_reg->SetFlagC(1);
	m_reg->AddPC(1);
}

void Instructions::SED() {
	m_reg->SetFlagD(1);
	m_reg->AddPC(1);
}

void Instructions::CLD() {
	m_reg->SetFlagD(0);
	m_reg->AddPC(1);
}

void Instructions::CLC() {
	m_reg->SetFlagC(0);
	m_reg->AddPC(1);
}

void Instructions::CLV() {
	m_reg->SetFlagV(0);
	m_reg->AddPC(1);
}

void Instructions::LDA(u8 value, u8 length) {
	m_reg->SetA(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);

	if (m_mem->GetPageCrossed())
		m_cyclesExtra = 1;
}

void Instructions::LDX(u8 value, u8 length) {
	m_reg->SetX(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);

	if (m_mem->GetPageCrossed())
		m_cyclesExtra = 1;
}

void Instructions::LDY(u8 value, u8 length) {
	m_reg->SetY(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);

	if (m_mem->GetPageCrossed())
		m_cyclesExtra = 1;
}

void Instructions::STA(u16 address, u8 length) {
	m_mem->MemW(address, m_reg->GetA());
	m_reg->AddPC(length);
}

void Instructions::STX(u16 address, u8 length) {
	m_mem->MemW(address, m_reg->GetX());
	m_reg->AddPC(length);
}

void Instructions::STY(u16 address, u8 length) {
	m_mem->MemW(address, m_reg->GetY());
	m_reg->AddPC(length);
}

void Instructions::TSX() {
	u8 value = m_reg->GetS();
	m_reg->SetX(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::TAX() {
	u8 value = m_reg->GetA();
	m_reg->SetX(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::TAY() {
	u8 value = m_reg->GetA();
	m_reg->SetY(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::TXA() {
	u8 value = m_reg->GetX();
	m_reg->SetA(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::TYA() {
	u8 value = m_reg->GetY();
	m_reg->SetA(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::TXS() {
	m_reg->SetS(m_reg->GetX());
	m_reg->AddPC(1);
}

void Instructions::DEC(u16 address, u8 length) {
	u8 value = m_mem->MemR(address) - 1;
	m_mem->MemW(address, value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);
}

void Instructions::DEX() {
	u8 value = m_reg->GetX() - 1;
	m_reg->SetX(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::DEY() {
	u8 value = m_reg->GetY() - 1;
	m_reg->SetY(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::INC(u16 address, u8 length) {
	u8 value = m_mem->MemR(address) + 1;
	m_mem->MemW(address, value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);
}

void Instructions::INX() {
	u8 value = m_reg->GetX() + 1;
	m_reg->SetX(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::INY() {
	u8 value = m_reg->GetY() + 1;
	m_reg->SetY(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::LSR() {
	u8 bit0 = m_reg->GetA() & 0x01;
	u8 value = m_reg->GetA() >> 1;
	m_reg->SetA(value);
	m_reg->SetFlagC(bit0);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::LSR(u16 address, u8 length) {
	u8 bit0 = m_mem->MemR(address) & 0x01;
	u8 value = m_mem->MemR(address) >> 1;
	m_mem->MemW(address, value);
	m_reg->SetFlagC(bit0);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);
}

void Instructions::ROL() {
	u8 bit7  = (m_reg->GetA() & 0x80) >> 7;
	u8 value = (m_reg->GetA() << 1) | (m_reg->GetFlagC());
	m_reg->SetA(value);
	m_reg->SetFlagC(bit7);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::ROL(u16 address, u8 length) {
	u8 bit7  = (m_mem->MemR(address) & 0x80) >> 7;
	u8 value = (m_mem->MemR(address) << 1) | (m_reg->GetFlagC());
	m_mem->MemW(address, value);
	m_reg->SetFlagC(bit7);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);
}

void Instructions::ROR() {
	u8 bit0 = m_reg->GetA() & 0x01;
	u8 value = (m_reg->GetFlagC() << 7) | (m_reg->GetA() >> 1);
	m_reg->SetA(value);
	m_reg->SetFlagC(bit0);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::ROR(u16 address, u8 length) {
	u8 bit0 = m_mem->MemR(address) & 0x01;
	u8 value = (m_reg->GetFlagC() << 7) | (m_mem->MemR(address) >> 1);
	m_mem->MemW(address, value);
	m_reg->SetFlagC(bit0);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);
}

void Instructions::ASL() {
	u8 bit7  = (m_reg->GetA() & 0x80) >> 7;
	u8 value = (m_reg->GetA() << 1);
	m_reg->SetA(value);
	m_reg->SetFlagC(bit7);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::ASL(u16 address, u8 length) {
	u8 bit7  = (m_mem->MemR(address) & 0x80) >> 7;
	u8 value = (m_mem->MemR(address) << 1);
	m_mem->MemW(address, value);
	m_reg->SetFlagC(bit7);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);
}

void Instructions::BIT(u8 value, u8 length) {
	u8 result = m_reg->GetA() & value;
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagV(BIT6(value) >> 6);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);
}

void Instructions::NOP(u8 length) {
	m_reg->AddPC(length);
}
