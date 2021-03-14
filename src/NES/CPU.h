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

#ifndef __NES_CPU_H__
#define __NES_CPU_H__

#include "Registers.h"
#include "Memory.h"

namespace Nes {
    
    class Video;
    class Instructions;
    class Cartridge;
    class Pad;

    class CPU: public Registers, public Memory
    {
    private:
        unsigned long m_numInstructions;
        u8 m_lastCycles;
        int m_cycles;
        int m_cyclesFrame;
        Video *m_v;
        Pad *m_p;

    public:
        CPU(Video *v, Pad *p, Sound *s);
        CPU(Video *v, Pad *p, Cartridge *c, Sound *s);
        ~CPU();
        
        int  Execute(int cyclesToExecute);
        int  ExecuteOneFrame();
        void Reset();
        int  GetElapsedCycles();
        void LoadStateFromRAM(std::istream *stream);
        void SaveStateToRAM(std::ostream *stream);
        void SaveState(std::string saveDirectory, int numSlot);
        void LoadState(std::string loadDirectory, int numSlot);
        
    private:
        void Init(Video *v, Pad *p);
        void ResetGlobalVariables();
        void Interrupts(bool NMI, Instructions &inst);
        
        u8 ExecuteOpcode(u8 opcode, Instructions &inst);
        
        inline u16 AddressZeroPageIndexed(u8 regValue);
        inline u16 AddressZeroPage();
        inline u16 AddressIndexedIndirect();
        inline u16 AddressIndirectIndexed();
        inline u16 AddressAbsoluteIndexed(u8 regValue);

        inline u8  Get8BitsInmValue();
        inline u16 Address16BitsInmValue();
        inline u8 Get16BitsInmValue();
        inline u8 GetZeroPageIndexed(u8 regValue);
        inline u8 GetZeroPage();
        inline u8 GetIndexedIndirect();
        inline u8 GetIndirectIndexed();
        inline u8 GetAbsoluteIndexed(u8 regValue);
    };
}
#endif
