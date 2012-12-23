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

#include <sstream>
#include <iomanip>
#include "Def.h"
#include "Sound.h"
#include "Video.h"
#include "CPU.h"
#include "Cartridge.h"
#include "Debugger.h"

using namespace std;

Debugger::Debugger(Sound *sound, Video *video, CPU *cpu, Cartridge *cartridge)
{
    this->sound = sound;
    this->video = video;
	this->cpu = cpu;
	this->cartridge = cartridge;
}

Debugger::~Debugger()
{
	
}

std::string Debugger::GetRegAF()
{
    return ToHex(cpu->Get_AF(), 4, '0');
}

std::string Debugger::GetRegBC()
{
    return ToHex(cpu->Get_BC(), 4, '0');
}

std::string Debugger::GetRegDE()
{
    return ToHex(cpu->Get_BC(), 4, '0');
}

std::string Debugger::GetRegHL()
{
    return ToHex(cpu->Get_HL(), 4, '0');
}

std::string Debugger::GetRegSP()
{
    return ToHex(cpu->Get_SP(), 4, '0');
}

std::string Debugger::GetRegPC()
{
    return ToHex(cpu->Get_PC(), 4, '0');
}

std::string Debugger::GetMem(WORD start, WORD end)
{
    start &= 0xFFF0;
    end = (end & 0xFFF0)+0x000F;
    
    stringstream ss;
    WORD row = start;
    while (row <= end)
    {
        ss << "0x";
        AppendHex(ss, row, 4, '0');
        ss << ": ";
        for (int i=0x0; i<0xF; i++)
        {
            BYTE value = cpu->MemR(row+i);
            AppendHex(ss, value, 2, '0');
            ss << ' ';
        }
        
        BYTE value = cpu->MemR(row+0xF);
        AppendHex(ss, value, 2, '0');
        if (row < end)
            ss << '\n';
        row += 0x10;
    }
    
    return ss.str();
}

void Debugger::GetBG(BYTE *buffer)
{
    
}

void Debugger::GetWindow(BYTE *buffer)
{
    
}

void Debugger::GetTiles(BYTE *buffer)
{
    
}

void Debugger::Step()
{
    
}

std::string Debugger::ToHex(int value, int width, char fill)
{
    stringstream ss;
    ss << setfill(fill) << setw(width) << uppercase << hex << value;
    return ss.str();
}

void Debugger::AppendHex(stringstream &ss, int value, int width, char fill)
{
    ss << setfill(fill) << setw(width) << uppercase << hex << (int)value;
}
