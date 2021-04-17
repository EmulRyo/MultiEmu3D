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

#ifndef __NES_DEBUGGER_H__
#define __NES_DEBUGGER_H__

#include "string"
#include "../Common/Debuggable.h"
#include "../Common/Types.h"

namespace Nes {
    
    class Sound;
    class Video;
    class CPU;
    class Cartridge;
    class Pad;

    class Debugger: public Debuggable {
    public:
        Debugger(Sound *sound, Video *video, CPU *cpu, Cartridge *cartridge, Pad *pad);
        ~Debugger();
        
        std::string GetRegA();
        std::string GetRegX();
        std::string GetRegY();
        std::string GetRegS();
        std::string GetRegP();
        std::string GetRegPC();

        bool GetFlag(int i);
        
        std::string GetRegs();
        
        std::string GetMem(u16 address);
        std::string GetMem(u16 start, u16 end);
        std::string GetVMem(u16 start, u16 end);

        std::string GetVideoNumFrames();
        std::string GetVideoX();
        std::string GetVideoY();
        std::string GetVideoScrollX();
        std::string GetVideoScrollY();
        std::string GetVideoAddress();

        std::string GetMapperID();
        std::string GetMapperName();
        std::string GetCartridgePRGBanks();
        std::string GetCartridgePRGBank0();
        std::string GetCartridgePRGBank1();
        std::string GetCartridgeCHRBanks();
        std::string GetCartridgeCHRBank0();
        std::string GetCartridgeCHRBank1();
        
        std::string Disassemble(u16 start, int numInstructions);
        std::string Disassemble(int numInstructions);
        void DisassembleNext(u16 &currentAddress, u16 &nextAddress, std::string &name, std::string &data);
        void DisassembleOne(u16 address, u16 &nextAddress, std::string &name, std::string &data);
        
        void Reset();
        void StepInto();
        bool ExecuteUntilNextLine();
        bool ExecuteOneFrame();
        void GetTiles(u8* buffer, int width, int height);
        void UpdatePad1(bool* buttonsState);
        
    private:
        Sound *m_sound;
        Video *m_video;
        CPU   *m_cpu;
        Cartridge *m_cartridge;
        Pad   *m_pad;
    };
}

#endif
