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
    return HexToString(m_cpu->GetA(), 2, '0');
}

std::string Debugger::GetRegX()
{
    return HexToString(m_cpu->GetX(), 2, '0');
}

std::string Debugger::GetRegY()
{
    return HexToString(m_cpu->GetY(), 2, '0');
}

std::string Debugger::GetRegS()
{
    return HexToString(m_cpu->GetS(), 2, '0');
}

std::string Debugger::GetRegP()
{
    return HexToString(m_cpu->GetP(), 2, '0');
}

std::string Debugger::GetRegPC()
{
    return HexToString(m_cpu->GetPC(), 4, '0');
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

std::string Debugger::GetVideoNumFrames() {
    return IntToString(m_video->GetNumFrames(), 3, '0');
}

std::string Debugger::GetVideoX() {
    return IntToString(m_video->GetX(), 3, '0');
}

std::string Debugger::GetVideoY() {
    return IntToString(m_video->GetY(), 3, '0');
}

std::string Debugger::GetVideoScrollX() {
    return IntToString(m_video->GetScrollX(), 3, '0');
}

std::string Debugger::GetVideoScrollY() {
    return IntToString(m_video->GetScrollY(), 3, '0');
}

std::string Debugger::GetVideoCurrentAddress() {
    return HexToString(m_video->GetCurrentAddress(), 4, '0');
}

std::string Debugger::GetVideoCoarseX() {
    return IntToString(m_video->GetCurrentAddress() & 0x1F, 2, '0');
}

std::string Debugger::GetVideoCoarseY() {
    return IntToString((m_video->GetCurrentAddress() >> 5) & 0x1F, 2, '0');
}

std::string Debugger::GetVideoFineY() {
    return IntToString((m_video->GetCurrentAddress() >> 12) & 0x07, 1, '0');
}

std::string Debugger::GetVideoNameTable() {
    return IntToString((m_video->GetCurrentAddress() >> 10) & 0x03, 1, '0');
}


std::string Debugger::GetVideoTempAddress() {
    return HexToString(m_video->GetTempAddress(), 4, '0');
}

std::string Debugger::GetVideoFineXScroll() {
    return IntToString(m_video->GetFineXScroll(), 1, '0');
}
std::string Debugger::GetVideoWriteToggle() {
    return IntToString(m_video->GetWriteToggle(), 1, '0');
}

std::string Debugger::GetVideoReg(u8 regID) {
    return HexToString(m_video->ReadReg(0x2000+regID, true), 2, '0');
}

std::string Debugger::GetVideoBaseNameTableAddress() {
    u8 value = (m_video->ReadReg(0x2000, true) & 0x03);
    u16 address = 0x2000 + (value * 0x400);
    return HexToString(address, 4, '0');
}

std::string Debugger::GetVideoVRAMAddressIncrement() {
    u8 value = (m_video->ReadReg(0x2000, true) & 0x04);
    return (value == 0) ? "+1" : "+32";
}

std::string Debugger::GetVideoSpritePatternTableAddress() {
    u8 value = (m_video->ReadReg(0x2000, true) & 0x08);
    return (value == 0) ? "$0000" : "$1000";
}

std::string Debugger::GetVideoBGPatternTableAddress() {
    u8 value = (m_video->ReadReg(0x2000, true) & 0x10);
    return (value == 0) ? "$0000" : "$1000";
}

std::string Debugger::GetVideoSpriteSize() {
    u8 value = (m_video->ReadReg(0x2000, true) & 0x20);
    return (value == 0) ? "8x8" : "8x16";
}

std::string Debugger::GetVideoGenerateNMI() {
    u8 value = (m_video->ReadReg(0x2000, true) & 0x80);
    return (value == 0) ? "False" : "True";
}

std::string Debugger::GetVideoGrayscale() {
    u8 value = (m_video->ReadReg(0x2001, true) & 0x01);
    return (value == 0) ? "False" : "True";
}

std::string Debugger::GetVideoShowBGLeft8() {
    u8 value = (m_video->ReadReg(0x2001, true) & 0x02);
    return (value == 0) ? "False" : "True";
}

std::string Debugger::GetVideoShowSpritesLeft8() {
    u8 value = (m_video->ReadReg(0x2001, true) & 0x04);
    return (value == 0) ? "False" : "True";
}

std::string Debugger::GetVideoShowBG() {
    u8 value = (m_video->ReadReg(0x2001, true) & 0x08);
    return (value == 0) ? "False" : "True";
}

std::string Debugger::GetVideoShowSprites() {
    u8 value = (m_video->ReadReg(0x2001, true) & 0x10);
    return (value == 0) ? "False" : "True";
}

std::string Debugger::GetVideoSpriteOverflow() {
    u8 value = (m_video->ReadReg(0x2002, true) & 0x20);
    return (value == 0) ? "False" : "True";
}

std::string Debugger::GetVideoSprite0Hit() {
    u8 value = (m_video->ReadReg(0x2002, true) & 0x40);
    return (value == 0) ? "False" : "True";
}

std::string Debugger::GetVideoVBlank() {
    u8 value = (m_video->ReadReg(0x2002, true) & 0x80);
    return (value == 0) ? "False" : "True";
}

std::string Debugger::GetMapperID() {
    return IntToString(m_cartridge->GetMapperNum(), 1, ' ');
}

std::string Debugger::GetMapperName() {
    return m_cartridge->GetMapperName();
}

std::string Debugger::GetCartridgePRGBanks() {
    return IntToString(m_cartridge->GetPRGBanks(), 1, '0');
}

std::string Debugger::GetCartridgePRGBank0() {
    return HexToString(m_cartridge->GetPRGBank0(), 2, '0');
}

std::string Debugger::GetCartridgePRGBank1() {
    return HexToString(m_cartridge->GetPRGBank1(), 2, '0');
}

std::string Debugger::GetCartridgeCHRBanks() {
    if (m_cartridge->GetCHRBanks() == 0)
        return "RAM";
    else
        return IntToString(m_cartridge->GetCHRBanks(), 1, '0');
}

std::string Debugger::GetCartridgeCHRBank0() {
    return HexToString(m_cartridge->GetCHRBank0(), 2, '0');
}

std::string Debugger::GetCartridgeCHRBank1() {
    return HexToString(m_cartridge->GetCHRBank1(), 2, '0');
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

std::string Debugger::GetOAMData(u16 start, u16 end)
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
            u8 value = m_video->OAMR(row + i);
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

bool Debugger::ExecuteUntilNextLine() {
    u16 currentLine = m_video->GetY();
    while (m_video->GetY() == currentLine) {
        m_cpu->Execute(1);
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
    m_pad->SetButtonsStatePad1(buttonsState);
}
