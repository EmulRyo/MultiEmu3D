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

#ifndef __CPU_H__
#define __CPU_H__

#include "Registers.h"
#include "Memory.h"

namespace MasterSystem {
    
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
        void SetGGMode(bool value);
        void LoadStateFromRAM(std::istream *stream);
        void SaveStateToRAM(std::ostream *stream);
        void SaveState(std::string saveDirectory, int numSlot);
        void LoadState(std::string loadDirectory, int numSlot);
        
    private:
        void Init(Video *v, Pad *p);
        void ResetGlobalVariables();
        void Interrupts(Instructions &inst);
        
        void ExecuteOpcode(u8 opcode, Instructions &inst);
        void OpcodeCB(Instructions &inst, bool &executed);
        void OpcodeDD(Instructions &inst, bool &executed);
        void OpcodeED(Instructions &inst, bool &executed);
        void OpcodeFD(Instructions &inst, bool &executed);
        void OpcodeDDCB(Instructions &inst, bool &executed);
        void OpcodeFDCB(Instructions &inst, bool &executed);
        
        u8  Get8BitsInmValue(u8 offset);
        u16 Get16BitsInmValue(u8 offset);
        u16 GetIXPlusD(u8 offset);
        u16 GetIYPlusD(u8 offset);

    };
}
#endif
