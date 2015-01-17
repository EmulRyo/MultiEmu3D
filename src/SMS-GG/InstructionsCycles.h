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

#ifndef __INSTRUCTIONSCYCLES_H__
#define __INSTRUCTIONSCYCLES_H__

#include "../Common/Types.h"

u8 GetInstructionCycles(u8 opcode);
u8 GetInstructionCondicionalCycles(u8 opcode);
u8 GetInstructionCyclesCB(u8 opcode);
u8 GetInstructionCyclesED(u8 opcode);
u8 GetInstructionCondicionalCyclesED(u8 opcode);
u8 GetInstructionCyclesXD(u8 opcode);
u8 GetInstructionCyclesXDCB(u8 opcode);

#endif
