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
#include "Pad.h"
#include "InstructionsLength.h"
#include "InstructionsName.h"
#include "Debugger.h"

using namespace std;
using namespace Nes;

Debugger::Debugger(Sound *sound, Video *video, CPU *cpu, Cartridge *cartridge, Pad *pad)
{
    m_sound = sound;
    m_video = video;
	m_cpu = cpu;
	m_cartridge = cartridge;
    m_pad = pad;
}

Debugger::~Debugger()
{
	
}

std::string Debugger::GetRegA()
{
    return ToHex(m_cpu->GetA(), 2, '0');
}

std::string Debugger::GetRegX()
{
    return ToHex(m_cpu->GetX(), 2, '0');
}

std::string Debugger::GetRegY()
{
    return ToHex(m_cpu->GetY(), 2, '0');
}

std::string Debugger::GetRegS()
{
    return ToHex(m_cpu->GetS(), 2, '0');
}

std::string Debugger::GetRegP()
{
    return ToHex(m_cpu->GetP(), 2, '0');
}

std::string Debugger::GetRegPC()
{
    return ToHex(m_cpu->GetPC(), 4, '0');
}

std::string Debugger::GetRegs()
{
    string regs;
    regs += "A: " + GetRegA() + "\n";
    regs += "X: " + GetRegX() + "\n";
    regs += "Y: " + GetRegY() + "\n";
    regs += "S: " + GetRegS() + "\n";
    regs += "P: " + GetRegP() + "\n";
    regs += "PC: " + GetRegPC() + "\n";
    
    return regs;
}

bool Debugger::GetFlag(int i) {
    return ((m_cpu->GetP() & (1 << i)) >> i) == 1 ? true : false;
}

std::string Debugger::GetMem(u16 address) {
    stringstream ss;
    
    u8 value = m_cpu->MemR(address, true);
    AppendHex(ss, value, 2, '0');
    return ss.str();
}

std::string Debugger::GetMem(u16 start, u16 end)
{
    start &= 0xFFF0;
    u16 end1 = end & 0xFFF0;
    u16 end2 = end1 + 0x000F;
    
    stringstream ss;
    unsigned int row = start;
    while (row <= end2)
    {
        AppendHex(ss, row, 4, '0');
        ss << ": ";
        for (int i=0x0; i<0xF; i++)
        {
            u8 value = m_cpu->MemR(row+i, true);
            AppendHex(ss, value, 2, '0');
            ss << ' ';
        }
        
        u8 value = m_cpu->MemR(row+0xF, true);
        AppendHex(ss, value, 2, '0');
        if (row < end1)
            ss << '\n';
        row += 0x10;
    }
    
    return ss.str();
}

std::string Debugger::GetVMem(u16 start, u16 end)
{
    start &= 0xFFF0;
    u16 end1 = end & 0xFFF0;
    u16 end2 = end1 + 0x000F;

    stringstream ss;
    unsigned int row = start;
    while (row <= end2)
    {
        AppendHex(ss, row, 4, '0');
        ss << ": ";
        for (int i = 0x0; i < 0xF; i++)
        {
            u8 value = m_video->MemR(row + i);
            AppendHex(ss, value, 2, '0');
            ss << ' ';
        }

        u8 value = m_video->MemR(row + 0xF);
        AppendHex(ss, value, 2, '0');
        if (row < end1)
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
        u8 opcode = m_cpu->MemR(address, true);
        ss << GetInstructionName(opcode);
        address += GetInstructionLength(opcode);
        
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
    u8 opcode1 = m_cpu->MemR(address, true);
    u8 opcode2 = m_cpu->MemR(address+1, true);
    ss1 << GetInstructionName(opcode1);
    length += GetInstructionLength(opcode1);
    
    for (int i=0; i<length; i++) {
        u16 nextData = m_cpu->MemR(address + i, true);
        AppendHex(ss2, nextData, 2, '0');
        if (i < length-1)
            ss2 << " ";
    }
    
    name = ss1.str();
    data = ss2.str();
    nextAddress = address + length;
}

void Debugger::Reset() {
    m_cpu->Reset();
}

void Debugger::StepInto() {
    m_cpu->Execute(1);
}

bool Debugger::ExecuteOneFrame() {
    int cycles = 0;
    while(cycles < NES_FRAME_CPU_CYCLES) {
        cycles += m_cpu->Execute(1);
        if (GetBreakpointNode(m_cpu->GetPC()))
            return false;
    }
    return true;
}

void Debugger::GetTiles(u8* buffer, int width, int height)
{
    int x, y, tile;
    u8* tmpBuffer;
    int widthSize = width * 3;
    int tilesInX = width / 8;
    int tilesInY = height / 8;
    int maxTiles = 512;

    y = 0;
    tile = 0;
    while ((y < tilesInY))
    {
        x = 0;
        tmpBuffer = buffer + (widthSize * y * 8);
        while ((x < tilesInX) && (tile < maxTiles))
        {
            m_video->GetTile(tmpBuffer, widthSize, tile);
            tmpBuffer += 8 * 3;
            tile++;
            x++;
        }
        y++;
    }
}

void Debugger::UpdatePad1(bool* buttonsState) {
    //m_pad->SetButtonsStatePad1(buttonsState);
}
