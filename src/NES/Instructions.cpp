/*
 This file is part of MultiEmu3D.
 
 MultiEmu3D is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MultiEmu3D is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MultiEmu3D.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <assert.h>
#include "Registers.h"
#include "Memory.h"
#include "Instructions.h"
#include "../Common/Bit.h"

using namespace std;
using namespace Nes;

#define _8bitsInmValue (m_mem->MemR(m_reg->GetPC() + 1))
#define _16bitsInmValue ((m_mem->MemR(m_reg->GetPC() + 2)) << 8) | m_mem->MemR(m_reg->GetPC() + 1)

Instructions::Instructions(Registers* reg, Memory* mem) {
	m_reg = reg;
	m_mem = mem;
}

Instructions::~Instructions(void) {
    
}

void Instructions::CMP(u8 value, u8 length) {
	u8 result = m_reg->GetA() - value;
	m_reg->SetFlagZ(result == 0 ? 1 : 0);
	m_reg->SetFlagN(BIT7(result) >> 7);
	m_reg->SetFlagC(m_reg->GetA() < value ? 0 : 1);
	m_reg->AddPC(length);
}