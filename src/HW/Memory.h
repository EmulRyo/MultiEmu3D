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

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <fstream>
#include "Cartridge.h"
#include "Sound.h"

#define SIZE_MEM       0x10000

class CPU;

class Memory
{
protected:
	Cartridge *m_c;
	Sound *m_s;
    CPU *m_cpu;

public:
	BYTE memory[SIZE_MEM];
public:
	Memory(CPU *cpu, Sound *s);
	~Memory();
	Memory *GetPtrMemory();
	void ResetMem();
	void LoadCartridge(Cartridge *c);
	void MemW(WORD direction, BYTE value);
	inline void MemWNoCheck(WORD address, BYTE value){ memory[address] = value; };
	inline BYTE MemR(WORD address)
	{
		return memory[address];
	}
	void SaveMemory(std::ofstream *file);
	void LoadMemory(std::ifstream *file);
};

#endif
