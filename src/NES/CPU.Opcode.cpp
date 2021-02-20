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

// #v
u8 CPU::Get8BitsInmValue(u8 offset) {
    return (MemR(GetPC() + offset));
}

u16 CPU::Get16BitsInmValue(u8 offset) {
    return ((MemR(GetPC() + offset + 1)) << 8) | MemR(GetPC() + offset);
}

// d,x | d,y
u16 CPU::GetZeroPageIndexed(u8 regValue) {
    return MemR((Get8BitsInmValue(1) + regValue) % 256);
}

// d
u16 CPU::GetZeroPage() {
    return MemR(Get8BitsInmValue(1));
}

// (d,x)
u16 CPU::GetIndexedIndirect() {
    return MemR(MemR((Get8BitsInmValue(1) + GetX()) % 256) + MemR((Get8BitsInmValue(1) + GetX() + 1) % 256) * 256);
}

// (d),y
u16 CPU::GetIndirectIndexed() {
    return MemR(MemR(Get8BitsInmValue(1)) + MemR((Get8BitsInmValue(1) + 1) % 256) * 256 + GetY());
}

// a,x | a,y
u16 CPU::GetAbsoluteIndexed(u8 regValue) {
    return MemR(Get16BitsInmValue(1) + regValue);
}


void CPU::ExecuteOpcode(u8 opcode, Instructions &inst) {
    
    
    switch(opcode)
    {
        case (0x10): inst.BPL(); break;

        case (0x30): inst.BMI(); break;

        case (0x50): inst.BVC(); break;

        case (0x70): inst.BVS(); break;

        case (0x90): inst.BCC(); break;

        case (0xB0): inst.BCS(); break;

        case (0xC0): inst.BNE(); break;
        case (0xC1): inst.CMP(GetIndexedIndirect(), 2); break;
        case (0xC5): inst.CMP(GetZeroPage(), 2); break;
        case (0xC9): inst.CMP(Get8BitsInmValue(1), 2); break;
        case (0xCD): inst.CMP(GetAbsoluteIndexed(0), 3); break;

        case (0xD0): inst.BEQ(); break;
        case (0xD1): inst.CMP(GetIndirectIndexed(), 2); break;
        case (0xD5): inst.CMP(GetZeroPageIndexed(GetX()), 2); break;
        case (0xD9): inst.CMP(GetAbsoluteIndexed(GetY()), 3); break;
        case (0xDD): inst.CMP(GetAbsoluteIndexed(GetX()), 3); break;
        
        default:
            stringstream out;
            out << "Error, instruction not implemented: 0x";
            out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << endl;
            cout << out.str();
            throw Exception(out.str());
            
    } // end switch
    
}

