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

#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#include "string"
#include "../Common/Debuggable.h"
#include "../Common/Types.h"

namespace MasterSystem {
    
    class Sound;
    class Video;
    class CPU;
    class Cartridge;
    class Pad;

    class Debugger: public Debuggable {
    public:
        Debugger(Sound *sound, Video *video, CPU *cpu, Cartridge *cartridge, Pad *pad);
        ~Debugger();
        
        std::string GetRegAF();
        std::string GetRegBC();
        std::string GetRegDE();
        std::string GetRegHL();
        
        std::string GetRegSP();
        std::string GetRegPC();
        
        std::string GetRegIX();
        std::string GetRegIY();
        
        std::string GetRegs();
        std::string GetVReg(u8 reg);
        std::string GetPal(u8 pal);
        
        bool GetFlag(int i);
        
        std::string GetMem(u16 address);
        std::string GetMem(u16 start, u16 end);
        std::string GetVMem(u16 start, u16 end);
        void GetBG(u8 *buffer);
        void GetTiles(u8 *buffer, int width, int height);
        std::string Disassemble(u16 start, int numInstructions);
        std::string Disassemble(int numInstructions);
        void DisassembleNext(u16 &currentAddress, u16 &nextAddress, std::string &name, std::string &data);
        void DisassembleOne(u16 address, u16 &nextAddress, std::string &name, std::string &data);
        u8   GetIPeriod();
        u8   GetVDPStatus();
        u16  GetLine();
        bool GetIE();
        bool GetIE0();
        bool GetIE1();
        u16  GetVideoAddress();
        u8   GetNumBank(u8 bank);
        bool GetRAMEnabled();
        u8   GetRAMNumBank();
        void UpdatePad1(bool *buttonsState);
        
        void Reset();
        void StepInto();
        bool ExecuteOneFrame();
        
    private:
        Sound *m_sound;
        Video *m_video;
        CPU   *m_cpu;
        Cartridge *m_cartridge;
        Pad   *m_pad;
    };
}

#endif
