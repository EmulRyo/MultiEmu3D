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

u8 CPU::Get8BitsInmValue(u8 offset) {
    return (MemR(GetPC() + offset));
}

u16 CPU::Get16BitsInmValue(u8 offset) {
    return ((MemR(GetPC() + offset + 1)) << 8) | MemR(GetPC() + offset);
}

void CPU::ExecuteOpcode(u8 opcode, Instructions &inst) {
    
    
    switch(opcode)
    {
        
        default:
            stringstream out;
            out << "Error, instruction not implemented: 0x";
            out << setfill('0') << setw(2) << uppercase << hex << (int)opcode << endl;
            cout << out.str();
            throw Exception(out.str());
            
    } // end switch
    
}

