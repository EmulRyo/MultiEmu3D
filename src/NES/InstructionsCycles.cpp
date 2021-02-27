/*
 This file is part of MarkSystem.
 
 MarkSystem is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MarkSystem is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MarkSystem.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "InstructionsCycles.h"

const u8 instructionCycles[] = {
    /* 00, 01, 02, 03, 04, 05, 06, 07, 08, 09, 0A, 0B, 0C, 0D, 0E, 0F*/
/*00*/ 07, 06, 02, 02, 02, 03, 05, 02, 03, 02, 02, 02, 02, 04, 06, 02,
/*10*/ 02, 05, 02, 02, 02, 04, 06, 02, 02, 04, 02, 02, 02, 04, 07, 02,
/*20*/ 06, 06, 02, 02, 03, 03, 05, 02, 04, 02, 02, 02, 04, 04, 06, 02,
/*30*/ 02, 05, 02, 02, 02, 04, 06, 02, 02, 04, 02, 02, 02, 04, 07, 02,
/*40*/ 06, 06, 02, 02, 02, 03, 05, 02, 03, 02, 02, 02, 03, 04, 06, 02,
/*50*/ 02, 05, 02, 02, 02, 04, 06, 02, 02, 04, 02, 02, 02, 04, 07, 02,
/*60*/ 06, 06, 02, 02, 02, 03, 05, 02, 04, 02, 02, 02, 05, 04, 06, 02,
/*70*/ 02, 05, 02, 02, 02, 04, 06, 02, 02, 04, 02, 02, 02, 04, 07, 02, 
/*80*/ 02, 06, 02, 02, 03, 03, 03, 02, 02, 02, 02, 02, 04, 04, 04, 02,
/*90*/ 02, 06, 02, 02, 04, 04, 04, 02, 02, 05, 02, 02, 02, 05, 02, 02,
/*A0*/ 02, 06, 02, 02, 03, 03, 03, 02, 02, 02, 02, 02, 04, 04, 04, 02,
/*B0*/ 02, 05, 02, 02, 04, 04, 04, 02, 02, 04, 02, 02, 04, 04, 04, 02,
/*C0*/ 02, 06, 02, 02, 03, 03, 05, 02, 02, 02, 02, 02, 04, 04, 06, 02,
/*D0*/ 02, 05, 02, 02, 02, 04, 06, 02, 02, 04, 02, 02, 02, 04, 07, 02,
/*E0*/ 02, 06, 02, 02, 03, 03, 05, 02, 02, 02, 02, 02, 04, 04, 06, 02,
/*F0*/ 02, 05, 02, 02, 02, 04, 06, 02, 02, 04, 02, 02, 02, 04, 07, 02
};

u8 Nes::GetInstructionCycles(u8 opcode) {
    return instructionCycles[opcode];
}
