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

#include <string.h>
#include "Cartridge.h"
#include "Sound.h"
#include "Pad.h"
#include "Memory.h"
#include "CPU.h"
#include "Video.h"
#include "Pad.h"

using namespace std;
using namespace Nes;

Memory::Memory(CPU *cpu, Video *v, Pad *p, Sound *s)
{
    m_cpu = cpu;
	m_c = NULL;
	m_s = s;
    m_video = v;
    m_pad = p;
	ResetMem();
}

Memory::~Memory()
{
}

Memory *Memory::GetPtrMemory() { return this; }

void Memory::SetCartridge(Cartridge *c)
{
	m_c = c;
}

void Memory::ResetMem() {
    memset(&memory, 0x00, SIZE_MEM);
    if (m_c)
        m_c->Reset();
}

void Memory::MemW(u16 address, u8 value)
{
    if (address < 0x0800)
        memory[address] = value;
    else if (address < 0x1000)
        memory[address - 0x0800] = value;
    else if (address < 0x1800)
        memory[address - 0x1000] = value;
    else if (address < 0x2000)
        memory[address - 0x1800] = value;
    else if (address < 0x4000)
        m_video->WriteReg(address, value);
    else if (address < 0x4014)
        throw(Exception("MemW APU registers"));
    else if (address == 0x4014)
        throw(Exception("MemW OAM DMA"));
    else if (address == 0x4015)
        throw(Exception("MemW APU registers"));
    else if (address < 0x4018)
        throw(Exception("MemW Joystick"));
    else if (address < 0x4020)
        throw(Exception("MemW test registers"));
    else
        m_c->Write(address, value);
}

void Memory::SaveMemory(ostream *stream)
{
	stream->write((char *)&memory[0x0000], SIZE_MEM);
}

void Memory::LoadMemory(istream *stream)
{
	stream->read((char *)&memory[0x0000], SIZE_MEM);
}
