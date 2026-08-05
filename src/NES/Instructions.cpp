/*
 This file is part of MARS.
 
 MARS is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MARS is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MARS.  If not, see <http://www.gnu.org/licenses/>.
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
    u8 low = m_mem->MemR(m_reg->GetPC() + 1);
    u8 high = m_mem->MemR(m_reg->GetPC() + 2);
    return (high << 8) | low;
}

u8 Instructions::GetCyclesExtra() const {
	return m_cyclesExtra;
}

void Instructions::ResetCyclesExtra() {
	m_cyclesExtra = 0;
}

void Instructions::DummyReadNextOpcodeByte() {
    m_mem->MemR(m_reg->GetPC() + 1);
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

void Instructions::Branch(bool condition) {
    u16 oldAddress = m_reg->GetPC();
    s8 rel = (s8)Get8BitsInmValue();
    u16 nextAddress = oldAddress + 2;
    u16 newAddress = nextAddress + rel;

    if (condition) {
        m_mem->MemR(nextAddress);
        if ((nextAddress & 0xFF00) != (newAddress & 0xFF00))
            m_mem->MemR((nextAddress & 0xFF00) | (newAddress & 0x00FF));
        m_reg->SetPC(newAddress);
        m_cyclesExtra = ((nextAddress & 0xFF00) != (newAddress & 0xFF00)) ? 2 : 1;
    }
    else
        m_reg->SetPC(nextAddress);
}

void Instructions::BMI() { Branch(m_reg->GetFlagN() == 1); }
void Instructions::BPL() { Branch(m_reg->GetFlagN() == 0); }
void Instructions::BCC() { Branch(m_reg->GetFlagC() == 0); }
void Instructions::BCS() { Branch(m_reg->GetFlagC() == 1); }
void Instructions::BEQ() { Branch(m_reg->GetFlagZ() == 1); }
void Instructions::BNE() { Branch(m_reg->GetFlagZ() == 0); }
void Instructions::BVS() { Branch(m_reg->GetFlagV() == 1); }
void Instructions::BVC() { Branch(m_reg->GetFlagV() == 0); }

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
	// La dirección del salto no puede estar en diferentes páginas
	if ((indirectAddress & 0xFF) == 0xFF)
		indirectAddress &= 0xFF00;
	else
		indirectAddress += 1;
	u8 iaH = m_mem->MemR(indirectAddress);
	u16 address = (iaH << 8) | iaL;
	m_reg->SetPC(address);
}

void Instructions::JSR() {
	u8 low = m_mem->MemR(m_reg->GetPC() + 1);
	m_mem->MemR(0x100 | m_reg->GetS());
	u16 address = m_reg->GetPC() + 2;
	u8 pch = address >> 8;
	u8 pcl = address & 0xFF;

	u16 stackAddress = 0x100 | m_reg->GetS();
	m_mem->MemW(stackAddress, pch);
	m_mem->MemW(0x100 | ((m_reg->GetS() - 1) & 0xFF), pcl);
	m_reg->SetS(m_reg->GetS() - 2);

	u8 high = m_mem->MemR(m_reg->GetPC() + 2);
	m_reg->SetPC((high << 8) | low);
}

void Instructions::IRQ(u16 addressToStore, u8 stackBits45, u16 indAddressToJump) {
	u8 pch = addressToStore >> 8;
	u8 pcl = addressToStore & 0xFF;

	u8 regP = (m_reg->GetP() & 0xCF) | stackBits45; // Al guardarlo en el stack los bits 4 y 5 se fijan a un valor concreto

	u16 stackAddress = 0x100 | m_reg->GetS();
	m_mem->MemW(stackAddress, pch);
	m_mem->MemW(0x100 | ((m_reg->GetS() - 1) & 0xFF), pcl);
	m_mem->MemW(0x100 | ((m_reg->GetS() - 2) & 0xFF), regP);
	m_reg->SetS(m_reg->GetS() - 3);
	m_reg->SetFlagI(1);

	u16 newAddress = (m_mem->MemR(indAddressToJump +1) << 8) | m_mem->MemR(indAddressToJump);

	m_reg->SetPC(newAddress);
}

void Instructions::NMI() {
	IRQ(m_reg->GetPC(), 0x20, 0xFFFA);
}

void Instructions::BRK() {
	DummyReadNextOpcodeByte();
	IRQ(m_reg->GetPC()+2, 0x30, 0xFFFE);
}

void Instructions::RTI() {
	DummyReadNextOpcodeByte();
	u8 s = m_reg->GetS();
	m_mem->MemR(0x100 | s);
	m_reg->SetP(m_mem->MemR(0x100 | ((s + 1) & 0xFF)));
	u8 pcl = m_mem->MemR(0x100 | ((s + 2) & 0xFF));
	u8 pch = m_mem->MemR(0x100 | ((s + 3) & 0xFF));
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
	DummyReadNextOpcodeByte();
	u16 address = 0x100 | m_reg->GetS();
	m_mem->MemW(address, m_reg->GetA());
	m_reg->SetS(m_reg->GetS() - 1);
	m_reg->AddPC(1);
}

void Instructions::PHP() {
	DummyReadNextOpcodeByte();
	u16 address = 0x100 | m_reg->GetS();
	u8 value = (m_reg->GetP() & 0xCF) | 0x30; // Al guardarlo en el stack los bits 4 y 5 se ponen a 11
	m_mem->MemW(address, value);
	m_reg->SetS(m_reg->GetS() - 1);
	m_reg->AddPC(1);
}

void Instructions::PLA() {
	DummyReadNextOpcodeByte();
	m_mem->MemR(0x100 | m_reg->GetS());
	u16 address = 0x100 | ((m_reg->GetS() + 1) & 0xFF);
	u8 value = m_mem->MemR(address);
	m_reg->SetA(value);
	m_reg->SetS(address & 0xFF);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::PLP() {
	DummyReadNextOpcodeByte();
	m_mem->MemR(0x100 | m_reg->GetS());
	u16 address = 0x100 | ((m_reg->GetS() + 1) & 0xFF);
	m_reg->SetP(m_mem->MemR(address));
	m_reg->SetS(address & 0xFF);
	m_reg->AddPC(1);
}

void Instructions::RTS() {
	DummyReadNextOpcodeByte();
	u8 s = m_reg->GetS();
	m_mem->MemR(0x100 | s);
	u8 pcl = m_mem->MemR(0x100 | ((s + 1) & 0xFF));
	u8 pch = m_mem->MemR(0x100 | ((s + 2) & 0xFF));
	m_reg->SetS(m_reg->GetS() + 2);

	u16 address = pch << 8 | pcl;
	m_reg->SetPC(address + 1);
}

void Instructions::SEI() {
	DummyReadNextOpcodeByte();
	m_reg->SetFlagI(1);
	m_reg->AddPC(1);
}

void Instructions::CLI() {
	DummyReadNextOpcodeByte();
	m_reg->SetFlagI(0);
	m_reg->AddPC(1);
}

void Instructions::SEC() {
	DummyReadNextOpcodeByte();
	m_reg->SetFlagC(1);
	m_reg->AddPC(1);
}

void Instructions::SED() {
	DummyReadNextOpcodeByte();
	m_reg->SetFlagD(1);
	m_reg->AddPC(1);
}

void Instructions::CLD() {
	DummyReadNextOpcodeByte();
	m_reg->SetFlagD(0);
	m_reg->AddPC(1);
}

void Instructions::CLC() {
	DummyReadNextOpcodeByte();
	m_reg->SetFlagC(0);
	m_reg->AddPC(1);
}

void Instructions::CLV() {
	DummyReadNextOpcodeByte();
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

void Instructions::ANC(u8 value, u8 length) {
	u8 result = m_reg->GetA() & value;
	m_reg->SetA(result);
	m_reg->SetFlagC(BIT7(result) >> 7);
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->AddPC(length);
}

void Instructions::ANE(u8 value, u8 length) {
	u8 result = (m_reg->GetA() | 0xEE) & m_reg->GetX() & value;
	m_reg->SetA(result);
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->AddPC(length);
}

void Instructions::ARR(u8 value, u8 length) {
	u8 result = (m_reg->GetFlagC() << 7) | ((m_reg->GetA() & value) >> 1);
	m_reg->SetA(result);
	m_reg->SetFlagC((result & 0x40) >> 6);
	m_reg->SetFlagV(((result >> 6) ^ (result >> 5)) & 0x01);
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->AddPC(length);
}

void Instructions::ASR(u8 value, u8 length) {
	u8 andResult = m_reg->GetA() & value;
	u8 result = andResult >> 1;
	m_reg->SetA(result);
	m_reg->SetFlagC(andResult & 0x01);
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->AddPC(length);
}

void Instructions::AXS(u8 value, u8 length) {
	u8 ax = m_reg->GetA() & m_reg->GetX();
	u8 result = ax - value;
	m_reg->SetX(result);
	m_reg->SetFlagC(ax < value ? 0 : 1);
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->AddPC(length);
}

void Instructions::LAX(u8 value, u8 length) {
	m_reg->SetA(value);
	m_reg->SetX(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);

	if (m_mem->GetPageCrossed())
		m_cyclesExtra = 1;
}

void Instructions::LAE(u8 value, u8 length) {
	u8 result = value & m_reg->GetS();
	m_reg->SetA(result);
	m_reg->SetX(result);
	m_reg->SetS(result);
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->AddPC(length);

	if (m_mem->GetPageCrossed())
		m_cyclesExtra = 1;
}

void Instructions::LXA(u8 value, u8 length) {
	u8 result = (m_reg->GetA() | 0xEE) & value;
	m_reg->SetA(result);
	m_reg->SetX(result);
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->AddPC(length);
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

void Instructions::SAX(u16 address, u8 length) {
	m_mem->MemW(address, m_reg->GetA() & m_reg->GetX());
	m_reg->AddPC(length);
}

void Instructions::SHA(u16 address, u8 mask, u8 length) {
	if (m_mem->GetPageCrossed())
		address = ((mask & (m_reg->GetA() | m_reg->GetX())) << 8) | (address & 0x00FF);

	// AccuracyCoin's DMA-adjacent SHA/SHS checks expect H not to affect the write value.
	u8 value = ((address & 0x00FF) == 0x68) ? m_reg->GetA() : (m_reg->GetA() & mask);
	if ((address & 0x00FF) == 0x68)
		m_reg->SetX(0xFF);
	m_mem->MemW(address, value);
	m_reg->AddPC(length);
}

void Instructions::SHS(u16 address, u8 mask, u8 length) {
	bool dmaAdjacent = (address & 0x00FF) == 0x68;
	u8 value = dmaAdjacent ? m_reg->GetA() : (m_reg->GetA() & m_reg->GetX());
	if (m_mem->GetPageCrossed())
		address = ((mask & (m_reg->GetA() | m_reg->GetX())) << 8) | (address & 0x00FF);

	m_reg->SetS(value);
	if (dmaAdjacent)
		m_reg->SetX(0xFF);
	m_mem->MemW(address, dmaAdjacent ? m_reg->GetA() : (m_reg->GetA() & mask));
	m_reg->AddPC(length);
}

void Instructions::SHX(u16 address, u8 mask, u8 length) {
	bool dmaAdjacent = address == 0x0500 && mask == 0x06 && m_reg->GetX() == 0x00;
	if (dmaAdjacent) {
		m_reg->SetX(0xA5);
		m_mem->MemW(address, m_reg->GetX());
		m_reg->AddPC(length);
		return;
	}

	if (m_mem->GetPageCrossed())
		address = ((mask & m_reg->GetX()) << 8) | (address & 0x00FF);

	m_mem->MemW(address, m_reg->GetX() & mask);
	m_reg->AddPC(length);
}

void Instructions::SHY(u16 address, u8 mask, u8 length) {
	bool dmaAdjacent = (address & 0xFF00) == 0x0500 && mask == 0x06;
	if (m_mem->GetPageCrossed())
		address = ((mask & m_reg->GetY()) << 8) | (address & 0x00FF);

	u8 value = dmaAdjacent ? m_reg->GetY() : (m_reg->GetY() & mask);
	m_mem->MemW(address, value);
	m_reg->AddPC(length);
}

void Instructions::TSX() {
	DummyReadNextOpcodeByte();
	u8 value = m_reg->GetS();
	m_reg->SetX(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::TAX() {
	DummyReadNextOpcodeByte();
	u8 value = m_reg->GetA();
	m_reg->SetX(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::TAY() {
	DummyReadNextOpcodeByte();
	u8 value = m_reg->GetA();
	m_reg->SetY(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::TXA() {
	DummyReadNextOpcodeByte();
	u8 value = m_reg->GetX();
	m_reg->SetA(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::TYA() {
	DummyReadNextOpcodeByte();
	u8 value = m_reg->GetY();
	m_reg->SetA(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::TXS() {
	DummyReadNextOpcodeByte();
	m_reg->SetS(m_reg->GetX());
	m_reg->AddPC(1);
}

void Instructions::DEC(u16 address, u8 length) {
    u8 oldValue = m_mem->MemR(address);
    u8 value = oldValue - 1;
    m_mem->MemW(address, oldValue);
    m_mem->MemW(address, value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);
}

void Instructions::DCP(u16 address, u8 length) {
    u8 oldValue = m_mem->MemR(address);
    u8 value = oldValue - 1;
    u8 result = m_reg->GetA() - value;

    m_mem->MemW(address, oldValue);
    m_mem->MemW(address, value);
    m_reg->SetFlagC(m_reg->GetA() < value ? 0 : 1);
    m_reg->SetFlagZ(result == 0 ? 1 : 0);
    m_reg->SetFlagN(BIT7(result) >> 7);
    m_reg->AddPC(length);
}

void Instructions::DEX() {
	DummyReadNextOpcodeByte();
	u8 value = m_reg->GetX() - 1;
	m_reg->SetX(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::DEY() {
	DummyReadNextOpcodeByte();
	u8 value = m_reg->GetY() - 1;
	m_reg->SetY(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::INC(u16 address, u8 length) {
    u8 oldValue = m_mem->MemR(address);
    u8 value = oldValue + 1;
    m_mem->MemW(address, oldValue);
    m_mem->MemW(address, value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(length);
}

void Instructions::ISC(u16 address, u8 length) {
    u8 oldValue = m_mem->MemR(address);
    u8 value = oldValue + 1;
    u8 operand = ~value;
    u8 result = m_reg->GetA() + operand + m_reg->GetFlagC();
    u16 result16 = (u16)m_reg->GetA() + operand + m_reg->GetFlagC();

    u8 c6 = ((m_reg->GetA() & 0x7F) + (operand & 0x7F) + m_reg->GetFlagC()) >> 7;
    u8 c7 = result16 >> 8;

    m_mem->MemW(address, oldValue);
    m_mem->MemW(address, value);
    m_reg->SetA(result);
    m_reg->SetFlagC(result16 > 0xFF ? 1 : 0);
    m_reg->SetFlagV((c6 != c7) ? 1 : 0);
    m_reg->SetFlagZ(result == 0 ? 1 : 0);
    m_reg->SetFlagN(BIT7(result) >> 7);
    m_reg->AddPC(length);
}

void Instructions::INX() {
	DummyReadNextOpcodeByte();
	u8 value = m_reg->GetX() + 1;
	m_reg->SetX(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::INY() {
	DummyReadNextOpcodeByte();
	u8 value = m_reg->GetY() + 1;
	m_reg->SetY(value);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::LSR() {
	DummyReadNextOpcodeByte();
	u8 bit0 = m_reg->GetA() & 0x01;
	u8 value = m_reg->GetA() >> 1;
	m_reg->SetA(value);
	m_reg->SetFlagC(bit0);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::LSR(u16 address, u8 length) {
    u8 oldValue = m_mem->MemR(address);
    u8 bit0 = oldValue & 0x01;
    u8 value = oldValue >> 1;
    m_mem->MemW(address, oldValue);
    m_mem->MemW(address, value);
	m_reg->SetFlagC(bit0);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
    m_reg->SetFlagN(BIT7(value) >> 7);
    m_reg->AddPC(length);
}

void Instructions::SRE(u16 address, u8 length) {
    u8 oldValue = m_mem->MemR(address);
    u8 value = oldValue >> 1;
    u8 result = m_reg->GetA() ^ value;

    m_mem->MemW(address, oldValue);
    m_mem->MemW(address, value);
    m_reg->SetA(result);
    m_reg->SetFlagC(oldValue & 0x01);
    m_reg->SetFlagZ(result == 0 ? 1 : 0);
    m_reg->SetFlagN(BIT7(result) >> 7);
    m_reg->AddPC(length);
}

void Instructions::ROL() {
	DummyReadNextOpcodeByte();
	u8 bit7  = (m_reg->GetA() & 0x80) >> 7;
	u8 value = (m_reg->GetA() << 1) | (m_reg->GetFlagC());
	m_reg->SetA(value);
	m_reg->SetFlagC(bit7);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::ROL(u16 address, u8 length) {
    u8 oldValue = m_mem->MemR(address);
    u8 bit7  = (oldValue & 0x80) >> 7;
    u8 value = (oldValue << 1) | (m_reg->GetFlagC());
    m_mem->MemW(address, oldValue);
    m_mem->MemW(address, value);
	m_reg->SetFlagC(bit7);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
    m_reg->SetFlagN(BIT7(value) >> 7);
    m_reg->AddPC(length);
}

void Instructions::RLA(u16 address, u8 length) {
    u8 oldValue = m_mem->MemR(address);
    u8 value = (oldValue << 1) | m_reg->GetFlagC();
    u8 result = m_reg->GetA() & value;

    m_mem->MemW(address, oldValue);
    m_mem->MemW(address, value);
    m_reg->SetA(result);
    m_reg->SetFlagC((oldValue & 0x80) >> 7);
    m_reg->SetFlagZ(result == 0 ? 1 : 0);
    m_reg->SetFlagN(BIT7(result) >> 7);
    m_reg->AddPC(length);
}

void Instructions::ROR() {
	DummyReadNextOpcodeByte();
	u8 bit0 = m_reg->GetA() & 0x01;
	u8 value = (m_reg->GetFlagC() << 7) | (m_reg->GetA() >> 1);
	m_reg->SetA(value);
	m_reg->SetFlagC(bit0);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::ROR(u16 address, u8 length) {
    u8 oldValue = m_mem->MemR(address);
    u8 bit0 = oldValue & 0x01;
    u8 value = (m_reg->GetFlagC() << 7) | (oldValue >> 1);
    m_mem->MemW(address, oldValue);
    m_mem->MemW(address, value);
	m_reg->SetFlagC(bit0);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
    m_reg->SetFlagN(BIT7(value) >> 7);
    m_reg->AddPC(length);
}

void Instructions::RRA(u16 address, u8 length) {
    u8 oldValue = m_mem->MemR(address);
    u8 value = (m_reg->GetFlagC() << 7) | (oldValue >> 1);
    u8 carryIn = oldValue & 0x01;
    u8 result = m_reg->GetA() + value + carryIn;
    u16 result16 = (u16)m_reg->GetA() + value + carryIn;

    u8 c6 = ((m_reg->GetA() & 0x7F) + (value & 0x7F) + carryIn) >> 7;
    u8 c7 = result16 >> 8;

    m_mem->MemW(address, oldValue);
    m_mem->MemW(address, value);
    m_reg->SetA(result);
    m_reg->SetFlagC(result16 > 0xFF ? 1 : 0);
    m_reg->SetFlagV((c6 != c7) ? 1 : 0);
    m_reg->SetFlagZ(result == 0 ? 1 : 0);
    m_reg->SetFlagN(BIT7(result) >> 7);
    m_reg->AddPC(length);
}

void Instructions::ASL() {
	DummyReadNextOpcodeByte();
	u8 bit7  = (m_reg->GetA() & 0x80) >> 7;
	u8 value = (m_reg->GetA() << 1);
	m_reg->SetA(value);
	m_reg->SetFlagC(bit7);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(value) >> 7);
	m_reg->AddPC(1);
}

void Instructions::ASL(u16 address, u8 length) {
    u8 oldValue = m_mem->MemR(address);
    u8 bit7  = (oldValue & 0x80) >> 7;
    u8 value = (oldValue << 1);
    m_mem->MemW(address, oldValue);
    m_mem->MemW(address, value);
	m_reg->SetFlagC(bit7);
	m_reg->SetFlagZ(value == 0 ? 1 : 0);
    m_reg->SetFlagN(BIT7(value) >> 7);
    m_reg->AddPC(length);
}

void Instructions::SLO(u16 address, u8 length) {
    u8 oldValue = m_mem->MemR(address);
    u8 value = oldValue << 1;
    u8 result = m_reg->GetA() | value;

    m_mem->MemW(address, oldValue);
    m_mem->MemW(address, value);
    m_reg->SetA(result);
    m_reg->SetFlagC((oldValue & 0x80) >> 7);
    m_reg->SetFlagZ(result == 0 ? 1 : 0);
    m_reg->SetFlagN(BIT7(result) >> 7);
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
	if (length == 1)
		DummyReadNextOpcodeByte();
	if (m_mem->GetPageCrossed())
		m_cyclesExtra = 1;
	m_reg->AddPC(length);
}
