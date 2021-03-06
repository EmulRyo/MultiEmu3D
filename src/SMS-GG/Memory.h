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

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <fstream>
#include "Cartridge.h"
#include "Sound.h"

#define SIZE_RAM   0x2000
#define SIZE_MEM (SIZE_RAM)

namespace MasterSystem {
    
    class CPU;
    class Video;
    class Pad;

    class Memory
    {
    protected:
        Cartridge *m_c;
        Sound *m_s;
        CPU *m_cpu;
        Video *m_video;
        Pad *m_pad;
        bool m_GameGear;

    public:
        Memory(CPU *cpu, Video *v, Pad *pad, Sound *s);
        ~Memory();
        Memory *GetPtrMemory();
        void ResetMem();
        void SetCartridge(Cartridge *c);
        void MemW(u16 direction, u8 value);
        inline u8 MemR(u16 address)
        {
            if (address < 0xC000)
                return m_c->Read(address);
            else if (address < 0xE000)
                return memory[address-0xC000];
            else
                return memory[address-0xE000];
        }
        void PortW(u8 port, u8 value);
        u8   PortR(u8 port);
        void SaveMemory(std::ostream *stream);
        void LoadMemory(std::istream *stream);
        
    private:
        u8 memory[SIZE_MEM];
        u8 m_GGRegs[7];
        
        void MemoryControlW(u8 value);
    };
}

#endif
