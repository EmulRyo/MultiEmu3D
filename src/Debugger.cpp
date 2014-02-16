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

std::string Debugger::GetRegs()
{
    string regs;
    regs += "AF: " + GetRegAF() + "\n";
    regs += "BC: " + GetRegBC() + "\n";
    regs += "DE: " + GetRegDE() + "\n";
    regs += "HL: " + GetRegHL() + "\n";
    regs += "PC: " + GetRegPC() + "\n";
    regs += "SP: " + GetRegSP();
    
    return regs;
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

std::string Debugger::GetMemVRam(WORD start, WORD end, int slot)
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
            BYTE value = cpu->MemRVRam(row+i, slot);
            AppendHex(ss, value, 2, '0');
            ss << ' ';
        }
        
        BYTE value = cpu->MemRVRam(row+0xF, slot);
        AppendHex(ss, value, 2, '0');
        if (row < end)
            ss << '\n';
        row += 0x10;
    }
    
    return ss.str();
}

std::string Debugger::GetMemPalette(int sprite, int number)
{
    int address = BGP_OFFSET;
    stringstream ss;
    
    number &= 0x07;
    if (sprite)
        address = OBP_OFFSET;
    
    address += number*8;
    
    ss << "Palette " << number << ": ";
    
    for (int i=0; i<4; i++)
    {
        WORD *value = (WORD *)((void *)&cpu->memory[address]);
        AppendHex(ss, *value, 4, '0');
        if (i < 3)
            ss << ", ";
        else
            ss << "\n";
        address += 2;
    }
    
    return ss.str();
}

void Debugger::GetColorPalette(int sprite, int number, BYTE palette[4][3])
{
    int address = BGP_OFFSET;
    
    number &= 0x07;
    if (sprite)
        address = OBP_OFFSET;
    
    address += number*8;
    
    video->GetColorPalette(palette, address);
}

void Debugger::GetBG(BYTE *buffer)
{
    
}

void Debugger::GetWindow(BYTE *buffer)
{
    
}

void Debugger::GetTiles(BYTE *buffer, int width, int height)
{
    int x, y, tile, slot;
    BYTE *tmpBuffer;
    int widthSize = width*3;
    int tilesInX = width / 8;
    int tilesInY = height / 8;
    
    y = 0;
    tile = 0;
    slot = 0;
    while ((y < tilesInY))
    {
        x = 0;
        tmpBuffer = buffer + (widthSize*y*8);
        while ((x < tilesInX) && (tile < 384))
        {
            video->GetTile(tmpBuffer, widthSize, tile, slot);
            tmpBuffer += 8*3;
            tile++;
            if ((slot == 0) && (tile >= 384))
            {
                tile = 0;
                slot = 1;
            }
            x++;
        }
        y++;
    }
}

void Debugger::Step()
{
    cpu->Execute(1);
}

void Debugger::ExecuteOneFrame() {
    cpu->ExecuteOneFrame();
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
