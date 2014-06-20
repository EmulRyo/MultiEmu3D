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

#define SIZE_RAM   0x2000
#define SIZE_MEM (SIZE_RAM)

class CPU;

class Memory
{
protected:
	Cartridge *m_c;
	Sound *m_s;
    CPU *m_cpu;

public:
	u8 memory[SIZE_MEM];
public:
	Memory(CPU *cpu, Sound *s);
	~Memory();
	Memory *GetPtrMemory();
	void ResetMem();
	void LoadCartridge(Cartridge *c);
	void MemW(u16 direction, u8 value);
	inline void MemWNoCheck(u16 address, u8 value){ memory[address-0xC000] = value; };
	inline u8 MemR(u16 address)
	{
        if (address < 0xC000)
            return m_c->Read(address);
        else if (address < 0xE000)
            return memory[address-0xC000];
        else if (address < 0xFFF0)
            return memory[address-0xE000];
        else {
            printf("MemR address not controlled: %d\n", address);
            return 0;
        }
		
	}
	void SaveMemory(std::ofstream *file);
	void LoadMemory(std::ifstream *file);
};

#endif
