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

#include <sstream>
#include <iomanip>
#include "Def.h"
#include "Sound.h"
#include "Video.h"
#include "CPU.h"
#include "Cartridge.h"
#include "InstructionsDef.h"
#include "Debugger.h"

using namespace std;
using namespace GameBoy;

Debugger::Debugger(GameBoy::Sound *sound, GameBoy::Video *video, GameBoy::CPU *cpu, GameBoy::Cartridge *cartridge)
{
    m_sound = sound;
    m_video = video;
	m_cpu = cpu;
	m_cartridge = cartridge;
    m_firstBreakpoint = NULL;
    m_lastBreakpoint = NULL;
}

Debugger::~Debugger()
{
	ClearBreakpoints();
}

std::string Debugger::GetRegAF()
{
    return ToHex(m_cpu->GetAF(), 4, '0');
}

std::string Debugger::GetRegBC()
{
    return ToHex(m_cpu->GetBC(), 4, '0');
}

std::string Debugger::GetRegDE()
{
    return ToHex(m_cpu->GetDE(), 4, '0');
}

std::string Debugger::GetRegHL()
{
    return ToHex(m_cpu->GetHL(), 4, '0');
}

std::string Debugger::GetRegSP()
{
    return ToHex(m_cpu->GetSP(), 4, '0');
}

std::string Debugger::GetRegPC()
{
    return ToHex(m_cpu->GetPC(), 4, '0');
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

std::string Debugger::GetMem(u16 address) {
    stringstream ss;
    
    u8 value = m_cpu->MemR(address);
    AppendHex(ss, value, 2, '0');
    return ss.str();
}

std::string Debugger::GetMem(u16 start, u16 end)
{
    start &= 0xFFF0;
    end = (end & 0xFFF0)+0x000F;
    
    stringstream ss;
    unsigned int row = start;
    while (row <= end)
    {
        AppendHex(ss, row, 4, '0');
        ss << ": ";
        for (int i=0x0; i<0xF; i++)
        {
            u8 value = m_cpu->MemR(row+i);
            AppendHex(ss, value, 2, '0');
            ss << ' ';
        }
        
        u8 value = m_cpu->MemR(row+0xF);
        AppendHex(ss, value, 2, '0');
        if (row < (end & 0xFFF0))
            ss << '\n';
        row += 0x10;
    }
    
    return ss.str();
}

std::string Debugger::Disassemble(u16 start, int numInstructions) {
    stringstream ss;
    
    int processed = 0;
    unsigned int address = start;
    while ((processed < numInstructions) && (address <= 0xFFFF))
    {
        ss << "0x";
        AppendHex(ss, address, 4, '0');
        ss << ": ";
        u8 opcode = m_cpu->MemR(address);
        if (opcode != 0xCB) {
            ss << GetInstructionName(opcode);
            address += GetInstructionLength(opcode);
        }
        else {
            opcode = m_cpu->MemR(address+1);
            ss << GetInstructionCBName(opcode);
            address += 2;
        }
        
        processed++;
        
        if (processed < numInstructions)
            ss << '\n';
    }
    
    return ss.str();
}

std::string Debugger::Disassemble(int numInstructions) {
    return Disassemble(m_cpu->GetPC(), numInstructions);
}

void Debugger::DisassembleNext(u16 &currentAddress, u16 &nextAddress, std::string &name, std::string &data) {
    currentAddress = m_cpu->GetPC();
    DisassembleOne(currentAddress, nextAddress, name, data);
}

void Debugger::DisassembleOne(u16 address, u16 &nextAddress, std::string &name, std::string &data) {
    stringstream ss1, ss2;
    
    int length = 0;
    u8 opcode = m_cpu->MemR(address);
    if (opcode != 0xCB) {
        ss1 << GetInstructionName(opcode);
        length += GetInstructionLength(opcode);
    }
    else {
        opcode = m_cpu->MemR(address+1);
        ss1 << GetInstructionCBName(opcode);
        length += 2;
    }
    
    for (int i=0; i<length; i++) {
        u16 nextData = m_cpu->MemR(address + i);
        AppendHex(ss2, nextData, 2, '0');
        if (i < length-1)
            ss2 << " ";
    }
    
    name = ss1.str();
    data = ss2.str();
    nextAddress = address + length;
}

std::string Debugger::GetMemVRam(u16 start, u16 end, int slot)
{
    start &= 0xFFF0;
    end = (end & 0xFFF0)+0x000F;
    
    stringstream ss;
    unsigned int row = start;
    while (row <= end)
    {
        ss << "0x";
        AppendHex(ss, row, 4, '0');
        ss << ": ";
        for (int i=0x0; i<0xF; i++)
        {
            u8 value = m_cpu->MemRVRam(row+i, slot);
            AppendHex(ss, value, 2, '0');
            ss << ' ';
        }
        
        u8 value = m_cpu->MemRVRam(row+0xF, slot);
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
        u16 *value = (u16 *)((void *)&m_cpu->memory[address]);
        AppendHex(ss, *value, 4, '0');
        if (i < 3)
            ss << ", ";
        else
            ss << "\n";
        address += 2;
    }
    
    return ss.str();
}

void Debugger::GetColorPalette(int sprite, int number, u8 palette[4][3])
{
    int address = BGP_OFFSET;
    
    number &= 0x07;
    if (sprite)
        address = OBP_OFFSET;
    
    address += number*8;
    
    m_video->GetColorPalette(palette, address);
}

void Debugger::GetBG(u8 *buffer)
{
    
}

void Debugger::GetWindow(u8 *buffer)
{
    
}

void Debugger::GetTiles(u8 *buffer, int width, int height)
{
    int x, y, tile, slot;
    u8 *tmpBuffer;
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
            m_video->GetTile(tmpBuffer, widthSize, tile, slot);
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

void Debugger::Reset() {
    m_cpu->Reset();
}

void Debugger::StepInto() {
    m_cpu->Execute(1);
}

bool Debugger::ExecuteOneFrame() {
    int cycles = 0;
    while(cycles < GB_FRAME_CYCLES) {
        cycles += m_cpu->Execute(1);
        if (GetBreakpointNode(m_cpu->GetPC()))
            return false;
    }
    return true;
}
