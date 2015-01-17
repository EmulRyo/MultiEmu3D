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
#include "Def.h"

#define SIZE_MAIN       0x10000
#define SIZE_WRAMCOLOR  0x8000
#define SIZE_VRAMCOLOR  0x4000
#define SIZE_BGPCOLOR   0x40    //BG Palettes Color
#define SIZE_OBPCOLOR   0x40    //Sprite Palettes Color

#define SIZE_MEM (SIZE_MAIN + SIZE_WRAMCOLOR + SIZE_VRAMCOLOR + SIZE_BGPCOLOR + SIZE_OBPCOLOR)

#define WRAM_OFFSET     SIZE_MAIN
#define VRAM_OFFSET     (WRAM_OFFSET + SIZE_WRAMCOLOR)
#define BGP_OFFSET      (VRAM_OFFSET + SIZE_VRAMCOLOR)
#define OBP_OFFSET      (BGP_OFFSET  + SIZE_OBPCOLOR)

namespace GameBoy {
    
    class CPU;
    
    class Memory {
    protected:
        Cartridge *m_c;
        Sound *m_s;
        CPU *m_cpu;
        bool m_colorMode;
        bool m_hdmaActive;
    private:
        u8 *m_wRam;
        u8 *m_vRam;
        void OamDmaTransfer(u8 direction);
        u8 VRamDmaTransfer(u8 value);
    public:
        u8 memory[SIZE_MEM];
    public:
        Memory(CPU *cpu, Sound *s);
        ~Memory();
        Memory *GetPtrMemory();
        void ResetMem();
        void LoadCartridge(Cartridge *c);
        void MemW(u16 direction, u8 value);
        inline void MemWNoCheck(u16 address, u8 value){ memory[address] = value; };
        inline u8 MemR(u16 address)
        {
            if ((address < 0x8000) || ((address >=0xA000) && (address < 0xC000)))
                return m_c->Read(address);
            else if ((address >= 0xFF10) && (address <= 0xFF3F))
                return m_s->ReadRegister(address);
            else if (m_colorMode && (address >= 0x8000) && address < 0xA000)
                return m_vRam[address - 0x8000];
            else if (m_colorMode && (address >= 0xD000) && address < 0xE000)
                return m_wRam[address - 0xD000];
            else if (m_colorMode && (address == BGPD))
            {
                u8 index = memory[BGPI] & 0x3F;
                return memory[BGP_OFFSET + index];
            }
            else if (m_colorMode && (address == OBPD))
            {
                u8 index = memory[OBPI] & 0x3F;
                return memory[OBP_OFFSET + index];
            }
            else
                return memory[address];
        }
        u8 MemRVRam(u16 address, int slot);
        u8 MemRWRam(u16 address, int slot);
        void UpdateHDMA();
        void SaveMemory(std::ofstream *file);
        void LoadMemory(std::ifstream *file);
    };
}

#endif
