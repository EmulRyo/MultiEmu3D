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
#include "InstructionsLength.h"

const u8 instructionLengths[] = {
    /* 00, 01, 02, 03, 04, 05, 06, 07, 08, 09, 0A, 0B, 0C, 0D, 0E, 0F*/
/*00*/  1,  2,  1,  2,  2,  2,  2,  2,  1,  2,  1,  2,  3,  3,  3,  3,
/*10*/  2,  2,  1,  2,  2,  2,  2,  2,  1,  3,  1,  3,  3,  3,  3,  3,
/*20*/  3,  2,  1,  2,  2,  2,  2,  2,  1,  2,  1,  2,  3,  3,  3,  3,
/*30*/  2,  2,  1,  2,  2,  2,  2,  2,  1,  3,  1,  3,  3,  3,  3,  3,
/*40*/  1,  2,  1,  2,  2,  2,  2,  2,  1,  2,  1,  2,  3,  3,  3,  3,
/*50*/  2,  2,  1,  2,  2,  2,  2,  2,  1,  3,  1,  3,  3,  3,  3,  3,
/*60*/  1,  2,  1,  2,  2,  2,  2,  2,  1,  2,  1,  2,  3,  3,  3,  3,
/*70*/  2,  2,  1,  2,  2,  2,  2,  2,  1,  3,  1,  3,  3,  3,  3,  3,
/*80*/  2,  2,  2,  2,  2,  2,  2,  2,  1,  2,  1,  2,  3,  3,  3,  3,
/*90*/  2,  2,  1,  2,  2,  2,  2,  2,  1,  3,  1,  3,  3,  3,  3,  3,
/*A0*/  2,  2,  2,  2,  2,  2,  2,  2,  1,  2,  1,  2,  3,  3,  3,  3,
/*B0*/  2,  2,  1,  2,  2,  2,  2,  2,  1,  3,  1,  3,  3,  3,  3,  3,
/*C0*/  2,  2,  2,  2,  2,  2,  2,  2,  1,  2,  1,  2,  3,  3,  3,  3,
/*D0*/  2,  2,  1,  2,  2,  2,  2,  2,  1,  3,  1,  3,  3,  3,  3,  3,
/*E0*/  2,  2,  2,  2,  2,  2,  2,  2,  1,  2,  1,  2,  3,  3,  3,  3,
/*F0*/  2,  2,  1,  2,  2,  2,  2,  2,  1,  3,  1,  3,  3,  3,  3,  3
};

u8 Nes::GetInstructionLength(u8 opcode) {
    return instructionLengths[opcode];
}
