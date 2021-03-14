/*
 This file is part of MarkSystem.
 
 MultiEmu3D is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MultiEmu3D is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MarkSystem.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iomanip>
#include <sstream>
#include "CPU.h"
#include "Instructions.h"
#include "InstructionsLength.h"
#include "InstructionsCycles.h"
#include "../Common/Exception.h"

using namespace std;
using namespace Nes;

// #i
u8 CPU::Get8BitsInmValue() {
    return (MemR(GetPC() + 1));
}

// a
u16 CPU::Address16BitsInmValue() {
    return ((MemR(GetPC() + 2)) << 8) | MemR(GetPC() + 1);
}

// a
u8 CPU::Get16BitsInmValue() {
    return MemR(Address16BitsInmValue());
}

// d,x | d,y
u16 CPU::AddressZeroPageIndexed(u8 regValue) {
    return (Get8BitsInmValue() + regValue) % 256;
}

// d,x | d,y
u8 CPU::GetZeroPageIndexed(u8 regValue) {
    return MemR(AddressZeroPageIndexed(regValue));
}

// d
u16 CPU::AddressZeroPage() {
    return Get8BitsInmValue();
}

// d
u8 CPU::GetZeroPage() {
    return MemR(AddressZeroPage());
}

// (d,x)
u16 CPU::AddressIndexedIndirect() {
    return MemR((Get8BitsInmValue() + GetX()) % 256) + MemR((Get8BitsInmValue() + GetX() + 1) % 256) * 256;
}

// (d,x)
u8 CPU::GetIndexedIndirect() {
    return MemR(AddressIndexedIndirect());
}

// (d),y
u16 CPU::AddressIndirectIndexed() {
    return MemR(Get8BitsInmValue()) + MemR((Get8BitsInmValue() + 1) % 256) * 256 + GetY();
}

// (d),y
u8 CPU::GetIndirectIndexed() {
    return MemR(AddressIndirectIndexed());
}

// a,x | a,y
u16 CPU::AddressAbsoluteIndexed(u8 regValue) {
    return Address16BitsInmValue() + regValue;
}

// a,x | a,y
u8 CPU::GetAbsoluteIndexed(u8 regValue) {
    return MemR(AddressAbsoluteIndexed(regValue));
}


u8 CPU::ExecuteOpcode(u8 opcode, Instructions &inst) {
    
    switch(opcode)
    {
        case (0x01): inst.ORA(GetIndexedIndirect(), 2); break;
        case (0x05): inst.ORA(GetZeroPage(), 2); break;
        case (0x08): inst.PHP(); break;
        case (0x09): inst.ORA(Get8BitsInmValue(), 2); break;
        case (0x0D): inst.ORA(Get16BitsInmValue(), 3); break;

        case (0x10): inst.BPL(); break;
        case (0x11): inst.ORA(GetIndirectIndexed(), 2); break;
        case (0x15): inst.ORA(GetZeroPageIndexed(GetX()), 2); break;
        case (0x18): inst.CLC(); break;
        case (0x19): inst.ORA(GetAbsoluteIndexed(GetY()), 3); break;
        case (0x1D): inst.ORA(GetAbsoluteIndexed(GetX()), 3); break;

        case (0x20): inst.JSR(); break;
        case (0x21): inst.AND(GetIndexedIndirect(), 2); break;
        case (0x25): inst.AND(GetZeroPage(), 2); break;
        case (0x26): inst.ROL(AddressZeroPage(), 2); break;
        case (0x28): inst.PLP(); break;
        case (0x29): inst.AND(Get8BitsInmValue(), 2); break;
        case (0x2A): inst.ROL(); break;
        case (0x2D): inst.AND(Get16BitsInmValue(), 3); break;
        case (0x2E): inst.ROL(Address16BitsInmValue(), 3); break;

        case (0x30): inst.BMI(); break;
        case (0x31): inst.AND(GetIndirectIndexed(), 2); break;
        case (0x35): inst.AND(GetZeroPageIndexed(GetX()), 2); break;
        case (0x36): inst.ROL(AddressZeroPageIndexed(GetX()), 2); break;
        case (0x38): inst.SEC(); break;
        case (0x39): inst.AND(GetAbsoluteIndexed(GetY()), 3); break;
        case (0x3D): inst.AND(GetAbsoluteIndexed(GetX()), 3); break;
        case (0x3E): inst.ROL(AddressAbsoluteIndexed(GetX()), 3); break;

        case (0x41): inst.EOR(GetIndexedIndirect(), 2); break;
        case (0x45): inst.EOR(GetZeroPage(), 2); break;
        case (0x46): inst.LSR(AddressZeroPage(), 2); break;
        case (0x49): inst.EOR(Get8BitsInmValue(), 2); break;
        case (0x48): inst.PHA(); break;
        case (0x4A): inst.LSR(); break;
        case (0x4C): inst.JMP(); break;
        case (0x4D): inst.EOR(Get16BitsInmValue(), 3); break;
        case (0x4E): inst.LSR(Address16BitsInmValue(), 3); break;

        case (0x50): inst.BVC(); break;
        case (0x51): inst.EOR(GetIndirectIndexed(), 2); break;
        case (0x55): inst.EOR(GetZeroPageIndexed(GetX()), 2); break;
        case (0x56): inst.LSR(AddressZeroPageIndexed(GetX()), 2); break;
        case (0x58): inst.CLI(); break;
        case (0x59): inst.EOR(GetAbsoluteIndexed(GetY()), 3); break;
        case (0x5D): inst.EOR(GetAbsoluteIndexed(GetX()), 3); break;
        case (0x5E): inst.LSR(AddressAbsoluteIndexed(GetX()), 3); break;

        case (0x60): inst.RTS(); break;
        case (0x61): inst.ADC(GetIndexedIndirect(), 2); break;
        case (0x65): inst.ADC(GetZeroPage(), 2); break;
        case (0x66): inst.ROR(AddressZeroPage(), 2); break;
        case (0x68): inst.PLA(); break;
        case (0x69): inst.ADC(Get8BitsInmValue(), 2); break;
        case (0x6A): inst.ROR(); break;
        case (0x6C): inst.JMPIndirect(); break;
        case (0x6D): inst.ADC(Get16BitsInmValue(), 3); break;
        case (0x6E): inst.ROR(Address16BitsInmValue(), 3); break;

        case (0x70): inst.BVS(); break;
        case (0x71): inst.ADC(GetIndirectIndexed(), 2); break;
        case (0x75): inst.ADC(GetZeroPageIndexed(GetX()), 2); break;
        case (0x76): inst.ROR(AddressZeroPageIndexed(GetX()), 2); break;
        case (0x78): inst.SEI(); break;
        case (0x79): inst.ADC(GetAbsoluteIndexed(GetY()), 3); break;
        case (0x7D): inst.ADC(GetAbsoluteIndexed(GetX()), 3); break;
        case (0x7E): inst.ROR(AddressAbsoluteIndexed(GetX()), 3); break;

        case (0x81): inst.STA(AddressIndexedIndirect(), 2); break;
        case (0x84): inst.STY(AddressZeroPage(), 2); break;
        case (0x85): inst.STA(AddressZeroPage(), 2); break;
        case (0x86): inst.STX(AddressZeroPage(), 2); break;
        case (0x88): inst.DEY(); break;
        case (0x8A): inst.TXA(); break;
        case (0x8C): inst.STY(Address16BitsInmValue(), 3); break;
        case (0x8D): inst.STA(Address16BitsInmValue(), 3); break;
        case (0x8E): inst.STX(Address16BitsInmValue(), 3); break;

        case (0x90): inst.BCC(); break;
        case (0x91): inst.STA(AddressIndirectIndexed(), 2); break;
        case (0x94): inst.STY(AddressZeroPageIndexed(GetX()), 2); break;
        case (0x95): inst.STA(AddressZeroPageIndexed(GetX()), 2); break;
        case (0x96): inst.STX(AddressZeroPageIndexed(GetY()), 2); break;
        case (0x98): inst.TYA(); break;
        case (0x99): inst.STA(AddressAbsoluteIndexed(GetY()), 3); break;
        case (0x9D): inst.STA(AddressAbsoluteIndexed(GetX()), 3); break;
        case (0x9A): inst.TXS(); break;

        case (0xA0): inst.LDY(Get8BitsInmValue(), 2); break;
        case (0xA1): inst.LDA(GetIndexedIndirect(), 2); break;
        case (0xA2): inst.LDX(Get8BitsInmValue(), 2); break;
        case (0xA4): inst.LDY(GetZeroPage(), 2); break;
        case (0xA5): inst.LDA(GetZeroPage(), 2); break;
        case (0xA6): inst.LDX(GetZeroPage(), 2); break;
        case (0xA8): inst.TAY(); break;
        case (0xA9): inst.LDA(Get8BitsInmValue(), 2); break;
        case (0xAA): inst.TAX(); break;
        case (0xAC): inst.LDY(Get16BitsInmValue(), 3); break;
        case (0xAD): inst.LDA(Get16BitsInmValue(), 3); break;
        case (0xAE): inst.LDX(Get16BitsInmValue(), 3); break;

        case (0xB0): inst.BCS(); break;
        case (0xB1): inst.LDA(GetIndirectIndexed(), 2); break;
        case (0xB4): inst.LDY(GetZeroPageIndexed(GetX()), 2); break;
        case (0xB5): inst.LDA(GetZeroPageIndexed(GetX()), 2); break;
        case (0xB6): inst.LDX(GetZeroPageIndexed(GetY()), 2); break;
        case (0xB8): inst.CLV(); break;
        case (0xB9): inst.LDA(GetAbsoluteIndexed(GetY()), 3); break;
        case (0xBC): inst.LDY(GetAbsoluteIndexed(GetX()), 3); break;
        case (0xBD): inst.LDA(GetAbsoluteIndexed(GetX()), 3); break;
        case (0xBE): inst.LDX(GetAbsoluteIndexed(GetY()), 3); break;
        case (0xBA): inst.TSX(); break;

        case (0xC0): inst.CPY(Get8BitsInmValue(), 2); break;
        case (0xC1): inst.CMP(GetIndexedIndirect(), 2); break;
        case (0xC4): inst.CPY(GetZeroPage(), 2); break;
        case (0xC5): inst.CMP(GetZeroPage(), 2); break;
        case (0xC6): inst.DEC(AddressZeroPage(), 2); break;
        case (0xC8): inst.INY(); break;
        case (0xC9): inst.CMP(Get8BitsInmValue(), 2); break;
        case (0xCC): inst.CPY(Get16BitsInmValue(), 3); break;
        case (0xCD): inst.CMP(Get16BitsInmValue(), 3); break;
        case (0xCA): inst.DEX(); break;
        case (0xCE): inst.DEC(Address16BitsInmValue(), 3); break;

        case (0xD0): inst.BNE(); break;
        case (0xD1): inst.CMP(GetIndirectIndexed(), 2); break;
        case (0xD5): inst.CMP(GetZeroPageIndexed(GetX()), 2); break;
        case (0xD6): inst.DEC(AddressZeroPageIndexed(GetX()), 2); break;
        case (0xD8): inst.CLD(); break;
        case (0xD9): inst.CMP(GetAbsoluteIndexed(GetY()), 3); break;
        case (0xDD): inst.CMP(GetAbsoluteIndexed(GetX()), 3); break;
        case (0xDE): inst.DEC(AddressAbsoluteIndexed(GetX()), 3); break;

        case (0xE0): inst.CPX(Get8BitsInmValue(), 2); break;
        case (0xE4): inst.CPX(GetZeroPage(), 2); break;
        case (0xE6): inst.INC(AddressZeroPage(), 2); break;
        case (0xE8): inst.INX(); break;
        case (0xEC): inst.CPX(Get16BitsInmValue(), 3); break;
        case (0xEE): inst.INC(Address16BitsInmValue(), 3); break;

        case (0xF0): inst.BEQ(); break;
        case (0xF6): inst.INC(AddressZeroPageIndexed(GetX()), 2); break;
        case (0xF8): inst.SED(); break;
        case (0xFE): inst.INC(AddressAbsoluteIndexed(GetX()), 3); break;
        
        default:
            stringstream out;
            out << "Error. Address: 0x";
            out << setfill('0') << setw(4) << uppercase << hex << (int)GetPC() << endl;
            out << "Instruction not implemented: 0x";
            out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << endl;
            cout << out.str();
            throw Exception(out.str());
            
    } // end switch
    
    return GetInstructionCycles(opcode);
}

