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
u16 CPU::Get16BitsInmValue() {
    return ((MemR(GetPC() + 2)) << 8) | MemR(GetPC() + 1);
}

// d,x | d,y
u16 CPU::GetZeroPageIndexed(u8 regValue) {
    return MemR((Get8BitsInmValue() + regValue) % 256);
}

// d
u16 CPU::GetZeroPage() {
    return MemR(Get8BitsInmValue());
}

// (d,x)
u16 CPU::GetIndexedIndirect() {
    return MemR(MemR((Get8BitsInmValue() + GetX()) % 256) + MemR((Get8BitsInmValue() + GetX() + 1) % 256) * 256);
}

// (d),y
u16 CPU::GetIndirectIndexed() {
    return MemR(MemR(Get8BitsInmValue()) + MemR((Get8BitsInmValue() + 1) % 256) * 256 + GetY());
}

// a,x | a,y
u16 CPU::GetAbsoluteIndexed(u8 regValue) {
    return MemR(Get16BitsInmValue() + regValue);
}


void CPU::ExecuteOpcode(u8 opcode, Instructions &inst) {
    
    
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
        case (0x19): inst.ORA(GetAbsoluteIndexed(GetY()), 3); break;
        case (0x1D): inst.ORA(GetAbsoluteIndexed(GetX()), 3); break;

        case (0x20): inst.JSR(); break;
        case (0x28): inst.PLP(); break;

        case (0x30): inst.BMI(); break;

        case (0x48): inst.PHA(); break;
        case (0x4C): inst.JMP(); break;

        case (0x50): inst.BVC(); break;
        case (0x58): inst.CLI(); break;

        case (0x60): inst.RTS(); break;
        case (0x61): inst.ADC(GetIndexedIndirect(), 2); break;
        case (0x65): inst.ADC(GetZeroPage(), 2); break;
        case (0x68): inst.PLA(); break;
        case (0x69): inst.ADC(Get8BitsInmValue(), 2); break;
        case (0x6C): inst.JMPIndirect(); break;
        case (0x6D): inst.ADC(Get16BitsInmValue(), 3); break;

        case (0x70): inst.BVS(); break;
        case (0x71): inst.ADC(GetIndirectIndexed(), 2); break;
        case (0x75): inst.ADC(GetZeroPageIndexed(GetX()), 2); break;
        case (0x78): inst.SEI(); break;
        case (0x79): inst.ADC(GetAbsoluteIndexed(GetY()), 3); break;
        case (0x7D): inst.ADC(GetAbsoluteIndexed(GetX()), 3); break;

        case (0x90): inst.BCC(); break;

        case (0xB0): inst.BCS(); break;

        case (0xC0): inst.CPY(Get8BitsInmValue(), 2); break;
        case (0xC1): inst.CMP(GetIndexedIndirect(), 2); break;
        case (0xC4): inst.CPY(GetZeroPage(), 2); break;
        case (0xC5): inst.CMP(GetZeroPage(), 2); break;
        case (0xC9): inst.CMP(Get8BitsInmValue(), 2); break;
        case (0xCC): inst.CPY(Get16BitsInmValue(), 3); break;
        case (0xCD): inst.CMP(Get16BitsInmValue(), 3); break;

        case (0xD0): inst.BNE(); break;
        case (0xD1): inst.CMP(GetIndirectIndexed(), 2); break;
        case (0xD5): inst.CMP(GetZeroPageIndexed(GetX()), 2); break;
        case (0xD9): inst.CMP(GetAbsoluteIndexed(GetY()), 3); break;
        case (0xDD): inst.CMP(GetAbsoluteIndexed(GetX()), 3); break;

        case (0xE0): inst.CPX(Get8BitsInmValue(), 2); break;
        case (0xE4): inst.CPX(GetZeroPage(), 2); break;
        case (0xEC): inst.CPX(Get16BitsInmValue(), 3); break;

        case (0xF0): inst.BEQ(); break;
        
        default:
            stringstream out;
            out << "Error, instruction not implemented: 0x";
            out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << endl;
            cout << out.str();
            throw Exception(out.str());
            
    } // end switch
    
}

