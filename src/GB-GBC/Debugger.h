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
#include "Def.h"

class Sound;
class Video;
class CPU;
class Cartridge;
struct BreakpointNode;

namespace GameBoy {
    class Debugger {
    public:
        Debugger(GameBoy::Sound *sound, Video *video, CPU *cpu, Cartridge *cartridge);
        ~Debugger();
        
        std::string GetRegAF();
        std::string GetRegBC();
        std::string GetRegDE();
        std::string GetRegHL();
        
        std::string GetRegSP();
        std::string GetRegPC();
        
        std::string GetRegs();
        
        std::string GetMem(u16 address);
        std::string GetMem(u16 start, u16 end);
        std::string GetMemVRam(u16 start, u16 end, int slot);
        std::string GetMemPalette(int sprite, int number);
        void GetBG(u8 *buffer);
        void GetWindow(u8 *buffer);
        void GetTiles(u8 *buffer, int width, int height);
        void GetColorPalette(int sprite, int number, u8 palette[4][3]);
        std::string Disassemble(u16 start, int numInstructions);
        std::string Disassemble(int numInstructions);
        void DisassembleNext(u16 &currentAddress, u16 &nextAddress, std::string &name, std::string &data);
        void DisassembleOne(u16 address, u16 &nextAddress, std::string &name, std::string &data);
        
        void Reset();
        void StepInto();
        bool ExecuteOneFrame();
        
        void AddBreakpoint(u16 address);
        void DelBreakpoint(u16 address);
        bool HasBreakpoint(u16 address);
        int  GetNumBreakpoints();
        u16 GetBreakpoint(int i);
        
        std::string ToHex(int value, int width, char fill);
        void AppendHex(std::stringstream &ss, int value, int width, char fill);
        
    private:
        GameBoy::Sound *m_sound;
        Video *m_video;
        CPU   *m_cpu;
        Cartridge *m_cartridge;
        BreakpointNode *m_firstBreakpoint;
        BreakpointNode *m_lastBreakpoint;
        
        BreakpointNode *GetBreakpointNode(u16 address);
        void ClearBreakpoints();
    };
}

#endif
