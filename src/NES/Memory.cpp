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
#include <assert.h>
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

bool Memory::GetPageCrossed() const {
    return m_pageCrossed;
}

void Memory::SetPageCrossed(bool value) {
    m_pageCrossed = value;
}

void Memory::PageCrossed(u16 address1, u16 address2) {
    m_pageCrossed = (address1 & 0xFF00) != (address2 & 0xFF00);
}

void Memory::SetCartridge(Cartridge *c)
{
	m_c = c;
}

void Memory::ResetMem() {
    memset(&memory, 0x00, SIZE_MEM);
    m_openBus = 0x00;
    m_internalBus = 0x00;
    m_dmcSampleAddressReg = 0x00;
    m_dmcDMAReadCounter = 0;
    m_dmcStatusReads = 0;
    m_dmcDMAActive = false;
    if (m_c)
        m_c->Reset();
}

// Cuando se lee de ciertas direcciones, el valor le�do no es el que se obtiene
// de la memoria, sino el valor del bus abierto (open bus), que es el �ltimo
// valor le�do o escrito en la memoria. Esto se debe a que en la arquitectura
// de la NES, algunas direcciones no corresponden a una memoria f�sica, sino que
// est�n conectadas al bus de datos sin una memoria detr�s.
u8 Memory::GetOpenBus() const {
    return m_openBus;
}

void Memory::SetOpenBus(u8 value) {
    m_openBus = value;
    m_internalBus = value;
}

void Memory::MemW(u16 address, u8 value)
{
    m_openBus = value;
    m_internalBus = value;

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
    else if (address < 0x4014) {
        if (address == 0x4012)
            m_dmcSampleAddressReg = value;
        m_s->MemW(address, value, m_cpu->GetElapsedCycles());
    }
    else if (address == 0x4014)
        m_cpu->OAMDMARequest(value);
    else if (address == 0x4015) {
        if (value & 0x10) {
            m_dmcDMAActive = true;
            m_dmcDMAReadCounter = 0;
            m_dmcStatusReads = 32;
        }
        else {
            m_dmcDMAActive = false;
            m_dmcStatusReads = 0;
        }
        m_s->MemW(address, value, m_cpu->GetElapsedCycles());
    }
    else if (address < 0x4018)
        m_pad->MemW(address, value);
    else if (address < 0x6000)
        return;
    else
        m_c->WritePRG(address, value);
}

void Memory::SaveMemory(ostream *stream) const
{
	stream->write((char *)&memory[0x0000], SIZE_MEM);
}

void Memory::LoadMemory(istream *stream)
{
	stream->read((char *)&memory[0x0000], SIZE_MEM);
}
