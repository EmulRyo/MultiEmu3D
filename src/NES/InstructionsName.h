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

#ifndef __NES_INSTRUCTIONSNAME_H__
#define __NES_INSTRUCTIONSNAME_H__

#include "../Common/Types.h"

namespace Nes {
	const char* GetInstructionName(u8 opcode);
	const char* GetInstructionCBName(u8 opcode);
	const char* GetInstructionDDName(u8 opcode);
	const char* GetInstructionEDName(u8 opcode);
	const char* GetInstructionFDName(u8 opcode);
	const char* GetInstructionDDCBName(u8 opcode);
	const char* GetInstructionFDCBName(u8 opcode);
}

#endif
