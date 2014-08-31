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
#include "Video.h"
#include "Pad.h"

using namespace std;

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

void Memory::LoadCartridge(Cartridge *c)
{
	m_c = c;
}

void Memory::ResetMem()
{
	memset(&memory, 0x00, SIZE_MEM);
    if (m_c)
        m_c->Reset();
}

void Memory::MemW(u16 address, u8 value)
{
    if (address < 0xC000)
        m_c->Write(address, value);
    else if (address < 0xE000)
        memory[address-0xC000] = value;
    else if (address < 0xFFFC)
        memory[address-0xE000] = value;
    else {
        m_c->Write(address, value);
        memory[address-0xE000] = value;
    }
}

void Memory::PortW(u8 port, u8 value) {
    u8 portF = port & 0xC1;
    switch (portF) {
        // 0x3F - Region detect
        case 0x01:
            m_pad->SetRegionData(value);
            break;
            
        // 0x7E - Sound
        // 0x7F - Sound
        case 0x40:
        case 0x41:
            m_s->WritePort(port, value, m_cpu->GetElapsedCycles());
            break;
            
        // 0xBE - Video data
        case 0x80:
            m_video->SetData(value);
            break;
            
        // 0xBF - Video registers
        case 0x81:
            m_video->SetControl(value);
            break;
            
        default:
            printf("PortW: 0x%X = 0x%X\n", port, value);
            break;
    }
};

u8 Memory::PortR(u8 port) {
    u8 portF = port & 0xC1;
    switch (portF) {
        case 0x40:
            return m_video->GetV();
            
        case 0x41:
            return m_video->GetH();
            
        case 0x80:
            return m_video->GetData();
            
        case 0x81:
            return m_video->GetControl();
            
        case 0xC0:
        case 0xC1:
            return m_pad->GetData(port);
            
        default:
            printf("PortR: 0x%X", port);
            return 0;
    }
}

void Memory::SaveMemory(ofstream * file)
{
	//file->write((char *)&memory[0x8000], 0x8000);
}

void Memory::LoadMemory(ifstream * file)
{
	//file->read((char *)&memory[0x8000], 0x8000);
}
