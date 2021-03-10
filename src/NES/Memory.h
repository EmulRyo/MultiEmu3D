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
        void ResetMem();
        void SetCartridge(Cartridge* c);
        void MemW(u16 direction, u8 value);
        inline u8 MemR(u16 address, bool debug=false)
        {
            if (address < 0x0800)
                return memory[address];
            else if (address < 0x1000)
                return memory[address - 0x0800];
            else if (address < 0x1800)
                return memory[address - 0x1000];
            else if (address < 0x2000)
                return memory[address - 0x1800];
            else if (address < 0x4000)
                return m_video->ReadReg(address, debug);
            else if (address < 0x4014)
                return m_s->MemR(address);
            else if (address == 0x4014) // OAM DMA
                throw(Exception("MemR OAM DMA"));
            else if (address == 0x4015)
                return m_s->MemR(address);
            else if (address < 0x4018)
                return m_pad->MemR(address);
            else if (address < 0x4020)
                throw(Exception("MemR test registers"));
            else
                return m_c->ReadPRG(address);
            return 0;
        }
        void SaveMemory(std::ostream* stream);
        void LoadMemory(std::istream* stream);

    protected:
        Cartridge *m_c;
        Sound *m_s;
        CPU *m_cpu;
        Video *m_video;
        Pad *m_pad;
        bool m_GameGear;
        
    private:
        u8 memory[SIZE_MEM];
    };
}

#endif
