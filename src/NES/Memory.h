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

#ifndef __NES_MEMORY_H__
#define __NES_MEMORY_H__

#include <fstream>
#include "Cartridge.h"
#include "Video.h"
#include "Pad.h"
#include "Sound.h"
#include "../Common/Exception.h"

#define SIZE_RAM   0x0800
#define SIZE_MEM (SIZE_RAM)

namespace Nes {
    
    class CPU;

    class Memory
    {
    public:
        Memory(CPU* cpu, Video* v, Pad* pad, Sound* s);
        ~Memory();
        Memory* GetPtrMemory();
        bool GetPageCrossed() const;
        void SetPageCrossed(bool value);
        void PageCrossed(u16 address1, u16 address2);
        void ResetMem();
        void SetCartridge(Cartridge* c);
        void MemW(u16 direction, u8 value);
        u8   GetOpenBus() const;
        void SetOpenBus(u8 value);
        inline u8 MemR(u16 address, bool debug=false)
        {
            u8 value = 0;
            bool updateBus = true;

            if (address < 0x0800)
                value = memory[address];
            else if (address < 0x1000)
                value = memory[address - 0x0800];
            else if (address < 0x1800)
                value = memory[address - 0x1000];
            else if (address < 0x2000)
                value = memory[address - 0x1800];
            else if (address < 0x4000)
                value = m_video->ReadReg(address, debug);
            else if (address < 0x4014) {
                if (m_dmcDMAActive && (++m_dmcDMAReadCounter % 8) == 0) {
                    u16 sampleAddress = 0xC000 + (m_dmcSampleAddressReg * 64);
                    value = m_c ? m_c->ReadPRG(sampleAddress) : 0x00;
                    m_openBus = value;
                }
                else
                    value = m_openBus;
                updateBus = false;
            }
            else if (address == 0x4014) {
                value = m_openBus;
                updateBus = false;
            }
            else if (address == 0x4015) {
                value = m_s->MemR(address) | (m_openBus & 0x20);
                if (m_dmcStatusReads > 0) {
                    value |= 0x10;
                    m_dmcStatusReads--;
                }
                else
                    m_dmcDMAActive = false;
                updateBus = false;
            }
            else if (address < 0x4018)
                value = m_pad->MemR(address) | (m_openBus & 0xE0);
            else if (address < 0x6000)
                value = m_openBus;
            else
                value = m_c->ReadPRG(address);

            if (updateBus)
                m_openBus = value;

            return value;
        }
        void SaveMemory(std::ostream* stream) const;
        void LoadMemory(std::istream* stream);

    protected:
        Cartridge *m_c;
        Sound *m_s;
        CPU *m_cpu;
        Video *m_video;
        Pad *m_pad;
        
    private:
        u8 memory[SIZE_MEM];
        u8 m_openBus;
        bool m_pageCrossed;
        u8 m_dmcSampleAddressReg;
        u8 m_dmcDMAReadCounter;
        u8 m_dmcStatusReads;
        bool m_dmcDMAActive;
    };
}

#endif
