/*
 This file is part of DMGBoy.
 
 DMGBoy is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 DMGBoy is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with DMGBoy.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __INSTRUCTIONSNAME_H__
#define __INSTRUCTIONSNAME_H__

#include "Def.h"

const char *GetInstructionName(u8 opcode);
const char *GetInstructionCBName(u8 opcode);
u8 GetInstructionLength(u8 opcode);
u8 GetInstructionCycles(u8 opcode);
u8 GetInstructionCondicionalCycles(u8 opcode);
u8 GetInstructionCyclesCB(u8 opcode);

#endif
