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

#include <string.h>
#include "Cartridge.h"
#include "Sound.h"
#include "Pad.h"
#include "Memory.h"
#include "CPU.h"

using namespace std;

Memory::Memory(CPU *cpu, Sound *s)
{
    m_cpu = cpu;
	m_c = NULL;
	m_s = s;
	ResetMem();
}

Memory::~Memory()
{
}

Memory *Memory::GetPtrMemory() { return this; }

void Memory::LoadCartridge(Cartridge *c)
{
	m_c = c;
}

void Memory::ResetMem()
{
	memset(&memory, 0x00, SIZE_MEM);
}

void Memory::MemW(u16 address, u8 value)
{
	memory[address] = value;
}

void Memory::SaveMemory(ofstream * file)
{
	//file->write((char *)&memory[0x8000], 0x8000);
}

void Memory::LoadMemory(ifstream * file)
{
	//file->read((char *)&memory[0x8000], 0x8000);
}
