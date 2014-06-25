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
    if (address < 0xC000)
        m_c->Write(address, value);
    else if (address < 0xE000)
        memory[address-0xC000] = value;
    else if (address < 0xFFF0)
        memory[address-0xE000] = value;
    else {
        printf("MemW address not controlled: 0x%X\n", address);
    }
}

void Memory::PortW(u8 port, u8 value) {
    // 0x3F - Language detect?
    // 0xDC: Joystick 1
    // 0xDD: Joystick 2
    // 0xDE - Unknown
    // 0xDF - Unknown
    // 0x7E - Sound
    // 0x7F - Sound
    // 0xBE - Video data
    // 0xBF - Video registers
    // 0xF0 - FM Sound register
    // 0xF1 - FM Sound data
    // 0xF2 - FM Sound detect
    if ((port != 0x7F) && (port != 0xBE) && (port != 0xBF))
        printf("PortW: 0x%X = 0x%X\n", port ,value);
};

u8 Memory::PortR(u8 port) {
    return 0;
}

void Memory::SaveMemory(ofstream * file)
{
	//file->write((char *)&memory[0x8000], 0x8000);
}

void Memory::LoadMemory(ifstream * file)
{
	//file->read((char *)&memory[0x8000], 0x8000);
}
