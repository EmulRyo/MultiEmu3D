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
    // 0xDE - Unknown
    // 0xDF - Unknown
    // 0x7E - Sound
    // 0x7F - Sound
    // 0xF0 - FM Sound register
    // 0xF1 - FM Sound data
    // 0xF2 - FM Sound detect
    switch (port) {
        // 0xBE - Video data
        case 0xBE:
            m_video->SetData(value);
            break;
        
        // 0xBF - Video registers
        case 0xBF:
            m_video->SetControl(value);
            break;
        
            // 0xDC: Joystick 1
            // 0xDD: Joystick 2
        case 0xDC:
        case 0xDD:
            m_pad->SetData(port, value);
            break;
            
        default:
            break;
    }
};

u8 Memory::PortR(u8 port) {
    
    switch (port) {
        case 0x7E:
            return m_video->GetLine();
            
        case 0xBE:
            return m_video->GetData();
            
        case 0xBF:
            return m_video->GetControl();
        
        case 0xDC:
        case 0xDD:
            return m_pad->GetData(port);
            
        default:
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
