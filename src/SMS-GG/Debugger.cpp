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
using namespace MasterSystem;

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

std::string Debugger::GetRegAF()
{
    return HexToString(m_cpu->GetAF(), 4, '0');
}

std::string Debugger::GetRegBC()
{
    return HexToString(m_cpu->GetBC(), 4, '0');
}

std::string Debugger::GetRegDE()
{
    return HexToString(m_cpu->GetDE(), 4, '0');
}

std::string Debugger::GetRegHL()
{
    return HexToString(m_cpu->GetHL(), 4, '0');
}

std::string Debugger::GetRegSP()
{
    return HexToString(m_cpu->GetSP(), 4, '0');
}

std::string Debugger::GetRegPC()
{
    return HexToString(m_cpu->GetPC(), 4, '0');
}

std::string Debugger::GetRegIX()
{
    return HexToString(m_cpu->GetIX(), 4, '0');
}

std::string Debugger::GetRegIY()
{
    return HexToString(m_cpu->GetIY(), 4, '0');
}

std::string Debugger::GetRegs()
{
    string regs;
    regs += "AF: " + GetRegAF() + "\n";
    regs += "BC: " + GetRegBC() + "\n";
    regs += "DE: " + GetRegDE() + "\n";
    regs += "HL: " + GetRegHL() + "\n";
    regs += "PC: " + GetRegPC() + "\n";
    regs += "SP: " + GetRegSP() + "\n";
    regs += "SP: " + GetRegIX() + "\n";
    regs += "SP: " + GetRegIY();
    
    return regs;
}

bool Debugger::GetFlag(int i) {
    u8 f = m_cpu->GetF();
    u8 value = f & (1 << i);
    return (value != 0);
}

std::string Debugger::GetVReg(u8 reg) {
    return HexToString(m_video->RegR(reg), 2, '0');
}

std::string Debugger::GetPal(u8 pal) {
    return HexToString(m_video->PalR(pal), 2, '0');
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
            u8 value = m_cpu->MemR(row+i);
            AppendHex(ss, value, 2, '0');
            ss << ' ';
        }
        
        u8 value = m_cpu->MemR(row+0xF);
        AppendHex(ss, value, 2, '0');
        if (row < end1)
            ss << '\n';
        row += 0x10;
    }
    
    return ss.str();
}

std::string Debugger::GetVMem(u16 start, u16 end)
{
    if (start > VDP_MEM-0x0010) {
        start = VDP_MEM-0x0010;
        end   = VDP_MEM-1;
    }
    
    if (end >= VDP_MEM)
        end = VDP_MEM-1;
    
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
            u8 value = m_video->MemR(row+i);
            AppendHex(ss, value, 2, '0');
            ss << ' ';
        }
        
        u8 value = m_video->MemR(row+0xF);
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
        u8 opcode = m_cpu->MemR(address);
        switch (opcode) {
            case 0xCB:
                opcode = m_cpu->MemR(address+1);
                ss << GetInstructionCBName(opcode);
                address += 2;
                break;
                
            default:
                ss << GetInstructionName(opcode);
                address += GetInstructionLength(opcode);
                break;
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
    u8 opcode1 = m_cpu->MemR(address);
    u8 opcode2 = m_cpu->MemR(address+1);
    switch (opcode1) {
        case 0xCB:
            ss1 << GetInstructionCBName(opcode2);
            length += GetInstructionCBLength(opcode2);
            break;
            
        case 0xDD:
            if (opcode2 == 0xCB) {
                u8 opcode3 = m_cpu->MemR(address+3);
                ss1 << GetInstructionDDCBName(opcode3);
                length += GetInstructionDDCBLength(opcode3);
            }
            else {
                ss1 << GetInstructionDDName(opcode2);
                length += GetInstructionDDLength(opcode2);
            }
            break;
            
        case 0xED:
            ss1 << GetInstructionEDName(opcode2);
            length += GetInstructionEDLength(opcode2);
            break;
        
        case 0xFD:
            if (opcode2 == 0xCB) {
                u8 opcode3 = m_cpu->MemR(address+3);
                ss1 << GetInstructionFDCBName(opcode3);
                length += GetInstructionFDCBLength(opcode3);
            }
            else {
                ss1 << GetInstructionFDName(opcode2);
                length += GetInstructionFDLength(opcode2);
            }
            break;
            
        default:
            ss1 << GetInstructionName(opcode1);
            length += GetInstructionLength(opcode1);
            break;
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

void Debugger::GetBG(u8 *buffer)
{
    
}

void Debugger::GetTiles(u8 *buffer, int width, int height)
{
    int x, y, tile;
    u8 *tmpBuffer;
    int widthSize = width*3;
    int tilesInX = width / 8;
    int tilesInY = height / 8;
    int maxTiles = 448;
    
    y = 0;
    tile = 0;
    while ((y < tilesInY))
    {
        x = 0;
        tmpBuffer = buffer + (widthSize*y*8);
        while ((x < tilesInX) && (tile < maxTiles))
        {
            m_video->GetTile(tmpBuffer, widthSize, tile);
            tmpBuffer += 8*3;
            tile++;
            x++;
        }
        y++;
    }
}

void Debugger::UpdatePad1(bool *buttonsState) {
    m_pad->SetButtonsStatePad1(buttonsState);
}

void Debugger::Reset() {
    m_cpu->Reset();
}

void Debugger::StepInto() {
    m_cpu->Execute(1);
}

bool Debugger::ExecuteOneFrame() {
    int cycles = 0;
    while(cycles < SMS_FRAME_CYCLES) {
        cycles += m_cpu->Execute(1);
        if (GetBreakpointNode(m_cpu->GetPC()))
            return false;
    }
    return true;
}

u8 Debugger::GetIPeriod() {
    return 228 - m_video->GetCyclesLine();
}

u8 Debugger::GetVDPStatus() {
    return m_video->GetStatus();
}

u16 Debugger::GetLine() {
    return m_video->GetLine();
}

bool Debugger::GetIE() {
    return m_cpu->GetIFF1();
}

bool Debugger::GetIE0() {
    return m_video->GetIE0();
}

bool Debugger::GetIE1() {
    return m_video->GetIE1();
}

u16 Debugger::GetVideoAddress() {
    return m_video->GetAddress();
}

u8 Debugger::GetNumBank(u8 bank) {
    return m_cartridge->GetNumBank(bank);
}

bool Debugger::GetRAMEnabled() {
    return m_cartridge->GetRAMEnabled();
}

u8 Debugger::GetRAMNumBank() {
    return m_cartridge->GetRAMNumBank();
}
