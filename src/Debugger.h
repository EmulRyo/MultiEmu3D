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

class Sound;
class Video;
class CPU;
class Cartridge;

class Debugger
{
public:
	Debugger(Sound *sound, Video *video, CPU *cpu, Cartridge *cartridge);
	~Debugger();
    
    std::string GetRegAF();
    std::string GetRegBC();
    std::string GetRegDE();
    std::string GetRegHL();
    
    std::string GetRegSP();
    std::string GetRegPC();
    
    std::string GetMem(WORD start, WORD end);
    void GetBG(BYTE *buffer);
    void GetWindow(BYTE *buffer);
    void GetTiles(BYTE *buffer);
    
    void Step();
    
private:
	Sound *sound;
    Video *video;
	CPU *cpu;
	Cartridge *cartridge;
    
    std::string ToHex(int value, int width, char fill);
    void AppendHex(std::stringstream &ss, int value, int width, char fill);
};

#endif
