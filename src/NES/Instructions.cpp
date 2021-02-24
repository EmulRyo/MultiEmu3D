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

void Instructions::ADC(u8 value, u8 length) {
	u8 result = m_reg->GetA() + value + m_reg->GetFlagC();

	m_reg->SetFlagC(result < m_reg->GetA() ? 1 : 0);
	m_reg->SetFlagV(BIT7(result) != BIT7(m_reg->GetA() ? 1 : 0));
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->AddPC(length);
}

void Instructions::BMI() {
	if (m_reg->GetFlagN() == 1)
		m_reg->AddPC((s8)Get8BitsInmValue());
	else
		m_reg->AddPC(2);
}

void Instructions::BPL() {
	if (m_reg->GetFlagN() == 0)
		m_reg->AddPC((s8)Get8BitsInmValue());
	else
		m_reg->AddPC(2);
}

void Instructions::BCC() {
	if (m_reg->GetFlagC() == 0)
		m_reg->AddPC((s8)Get8BitsInmValue());
	else
		m_reg->AddPC(2);
}

void Instructions::BCS() {
	if (m_reg->GetFlagC() == 1)
		m_reg->AddPC((s8)Get8BitsInmValue());
	else
		m_reg->AddPC(2);
}
;
void Instructions::BEQ() {
	if (m_reg->GetFlagZ() == 1)
		m_reg->AddPC((s8)Get8BitsInmValue());
	else
		m_reg->AddPC(2);
}

void Instructions::BNE() {
	if (m_reg->GetFlagZ() == 0)
		m_reg->AddPC((s8)Get8BitsInmValue());
	else
		m_reg->AddPC(2);
}

void Instructions::BVS() {
	if (m_reg->GetFlagV() == 1)
		m_reg->AddPC((s8)Get8BitsInmValue());
	else
		m_reg->AddPC(2);
}

void Instructions::BVC() {
	if (m_reg->GetFlagV() == 0)
		m_reg->AddPC((s8)Get8BitsInmValue());
	else
		m_reg->AddPC(2);
}

void Instructions::CMP(u8 value, u8 length) {
	u8 result = m_reg->GetA() - value;
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->SetFlagC(m_reg->GetA() < value ? 0 : 1);
	m_reg->AddPC(length);
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
	u8 iaH = m_mem->MemR(indirectAddress+1);
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

void Instructions::ORA(u8 value, u8 length) {
	u8 result = m_reg->GetA() | value;
	m_reg->SetA(result);
	m_reg->SetFlagZ(result == 0 ? 0 : 1);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->AddPC(length);
}

void Instructions::PHA() {
	u16 address = 0x100 | m_reg->GetS();
	m_mem->MemW(address, m_reg->GetA());
	m_reg->SetS(m_reg->GetS() - 1);
	m_reg->AddPC(1);
}

void Instructions::PHP() {
	u16 address = 0x100 | m_reg->GetS();
	m_mem->MemW(address, m_reg->GetP());
	m_reg->SetS(m_reg->GetS() - 1);
	m_reg->AddPC(1);
}

void Instructions::PLA() {
	u16 address = 0x100 | m_reg->GetS() + 1;
	m_reg->SetA(m_mem->MemR(address));
	m_reg->SetS(address & 0xFF);
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

void Instructions::SED() {
	m_reg->SetFlagD(1);
	m_reg->AddPC(1);
}

void Instructions::CLD() {
	m_reg->SetFlagD(0);
	m_reg->AddPC(1);
}

void Instructions::LDA(u8 value, u8 length) {
	m_reg->SetA(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);
}

void Instructions::STA(u16 address, u8 length) {
	m_mem->MemW(address, m_reg->GetA());
	m_reg->AddPC(length);
}
