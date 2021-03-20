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

#include <iostream>
#include "Cartridge.h"
#include "Video.h"
#include "../Common/Bit.h"
#include "../Common/IScreenDrawable.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define PPUCTRL	  0x2000
#define PPUMASK	  0x2001
#define PPUSTATUS 0x2002
#define OAMADDR	  0x2003
#define OAMDATA	  0x2004
#define PPUSCROLL 0x2005
#define PPUADDR	  0x2006
#define PPUDATA   0x2007

using namespace std;
using namespace Nes;

u8 PALETTE_2C02_NESTOPIA[] = {
/*        R0,  G0,  B0,   R1,  G1,  B1,   R2,  G2,  B2,   R3,  G3,  B3,   R4,  G4,  B4,   R5,  G5,  B5,   R6,  G6,  B6,   R7,  G7,  B7,   R8,  G8,  B8,   R9,  G9,  B9,   RA,  GA,  BA,   RB,  GB,  BB,   RC,  GC,  BC,   RD,  GD,  BD,   RE,  GE,  BE,   RF,  GF,  BF,  */
/* 00 */  84,  84,  84,    0,  30, 116,    8,  16, 144,   48,   0, 136,   68,   0, 100,   92,   0,  48,   84,   4,   0,   60,  24,   0,   32,  42,   0,    8,  58,   0,    0,  64,   0,    0,  60,   0,    0,  50,  60,    0,   0,   0,    0,   0,   0,    0,   0,   0,
/* 10 */ 152, 150, 152,    8,  76, 196,   48,  50, 236,   92,  30, 228,  136,  20, 176,  160,  20, 100,  152,  34,  32,  120,  60,   0,   84,  90,   0,   40, 114,   0,    8, 124,   0,    0, 118,  40,    0, 102, 120,    0,   0,   0,    0,   0,   0,    0,   0,   0,
/* 20 */ 236, 238, 236,   76, 154, 236,  120, 124, 236,  176,  98, 236,  228,  84, 236,  236,  88, 180,  236, 106, 100,  212, 136,  32,  160, 170,   0,  116, 196,   0,   76, 208,  32,   56, 204, 108,   56, 180, 204,   60,  60,  60,    0,   0,   0,    0,   0,   0,
/* 30 */ 236, 238, 236,  168, 204, 236,  188, 188, 236,  212, 178, 236,  236, 174, 236,  236, 174, 212,  236, 180, 176,  228, 196, 144,  204, 210, 120,  180, 222, 120,  168, 226, 144,  152, 226, 180,  160, 214, 228,  160, 162, 160,    0,   0,   0,    0,   0,   0
};

Video::Video(IScreenDrawable *screen)
{
    m_pixel = new VideoPixel();
    Reset();
	SetScreen(screen);
}

Video::~Video(void)
{
	delete m_pixel;
}

void Video::SetScreen(IScreenDrawable *screen)
{
    m_screen = screen;
}

void Video::SetCartridge(Cartridge* c) {
    m_cartridge = c;
}

void Video::RefreshScreen()
{
    m_screen->OnRefreshFalseScreen();
}

void Video::Reset() {
    m_x = 0;
    m_line = 0;
    m_cycles = 0;
    m_cyclesLine = 0;
    m_numFrames = 0;
    m_regs[ 0] = 0x00;
    m_regs[ 1] = 0x00;
    m_regs[ 2] = 0x00;
    m_regs[ 3] = 0x00;
    m_regs[ 4] = 0x00;
    m_regs[ 5] = 0x00;
    m_regs[ 6] = 0x00;
    m_regs[ 7] = 0x00;
    
    m_genLatch = 0;
    m_addressLatch = 0x0000;
    m_addressNumWrite = 0;
}

void Video::ClearScreen()
{
    if (m_screen) {
        m_screen->OnSizeChanged(0, 0, NES_SCREEN_W, NES_SCREEN_H);
        m_screen->OnClear();
    }
}

u8 Video::ReadReg(u16 address, bool debug) {
    address = ((address - 0x2000) % 8) + 0x2000;
    u8 regID = address & 0x07;
    if (debug)
        return m_regs[regID];

    if (address == PPUSTATUS) {
        u8 value = (m_regs[regID] & 0xE0) | (m_genLatch & 0x1F); // Los bits 0-4 se cogen del valor del latch
        m_regs[regID] = m_regs[regID] & 0x7F; // Al leer este registro se desactiva el bit 7 (V-Blank)
        m_addressLatch = 0x0000; // Y tambien se resetea el address latch
        m_addressNumWrite = 0;
        m_genLatch = m_regs[regID];
        return value;
    }
    else if (address == OAMDATA) {
        m_genLatch = m_regs[regID];
        return m_regs[regID];
    }
    else if (address == PPUDATA) {
        u8 value = MemR(m_addressLatch);
        u8 increment = (m_regs[PPUCTRL & 0x07] & 0x04) == 0 ? 1 : 32;
        m_addressLatch += increment;

        m_genLatch = value;
        return value;
    }
    else
        return m_genLatch;
}

void Video::WriteReg(u16 address, u8 value) {
    m_genLatch = value;
    address = ((address - 0x2000) % 8) + 0x2000;
    if (address == PPUADDR) {
        if ((m_addressNumWrite % 2) == 0) // Primera escritura, upper byte
            m_addressLatch = (value << 8) | (m_addressLatch & 0x00FF);
        else // Segunda escritura, lower byte
            m_addressLatch = (m_addressLatch & 0xFF00) | value;
        m_addressNumWrite++;
    }
    else if (address == PPUDATA) {
        MemW(m_addressLatch, value);
        u8 increment = (m_regs[PPUCTRL & 0x07] & 0x04) == 0 ? 1 : 32;
        m_addressLatch += increment;
    }
    else if (address == OAMADDR) {
        m_OAMAddress = value;
        m_regs[OAMDATA & 0x07] = m_OAM[m_OAMAddress];
    }
    else if (address == OAMDATA) {
        m_OAM[m_OAMAddress] = value;
        m_OAMAddress++;
    }
    
    if (address != PPUSTATUS) {
        u8 regID = address & 0x07;
        m_regs[regID] = value;
    }
}

bool Video::Update(u8 cpuCycles) {
    bool NMI = false;
    m_cycles += cpuCycles *3;
    
    if (m_cycles > NES_FRAME_PPU_CYCLES)
        OnEndFrame();
    
    m_cyclesLine += cpuCycles *3;
    u16 scanLineCycles = NES_SCANLINE_PPU_CYCLES;
    if (m_line == 261 && (m_numFrames % 2 == 1))
        scanLineCycles -= 1;

    if (m_cyclesLine > scanLineCycles) {
        m_x = 0;
        m_cyclesLine -= scanLineCycles;
        if (m_line == (NES_SCREEN_H-1))
            RefreshScreen();
        else if (m_line == (NES_SCREEN_H)) {
            // Set VBlank Flag
            u8 regID = PPUSTATUS & 0x07;
            m_regs[regID] |= 0x80;

            u8 ppuCtrlData = m_regs[PPUCTRL & 0x07];
            if ((ppuCtrlData & 0x80) > 0)
                NMI = true; // Si el bit 7 de PPUCTRL está activo, generar una Non-Maskable Interrupt
        }
        
        m_line++;
    }

    UpdatePixels();

    return NMI;
}

void Video::OnEndFrame() {
    m_cycles -= NES_FRAME_PPU_CYCLES;
    m_cyclesLine = 0;
    m_line = 0;
    m_regs[PPUSTATUS & 0x07] &= 0x7F; // Al inicio de cada frame se borra el bit de V-Blank
    m_numFrames++;
}

void Video::UpdatePixels() {
    if (m_line >= NES_SCREEN_H)
        return;

    u8 ppuCtrl = m_regs[PPUCTRL & 0x07];
    bool spriteSize16 = (ppuCtrl & 0x20) > 0 ? true : false;
    u16 BgPatternTableAddress = (ppuCtrl & 0x10) > 0 ? 0x1000 : 0x0000;
    u16 SpritePatternTableAddress = (ppuCtrl & 0x08) > 0 ? 0x1000 : 0x0000;
    u16 nameTableAddress = ((ppuCtrl & 0x03) * 0x400) + 0x2000;
    u16 attrTableAddress = nameTableAddress + 0x03C0;

    u16 maxX = m_cyclesLine - 1;
    if (maxX > NES_SCREEN_W)
        maxX = NES_SCREEN_W;

    for (u16 x = m_x; x < maxX; x++) {
        u8 tileCol = x / 8;
        u8 tileRow = m_line / 8;
        u16 nameTableOffset = tileRow * 32 + tileCol;
        u8 tileID = MemR(nameTableAddress + nameTableOffset);

        // Attribute Table
        u8 attrCol = x / 32;
        u8 attrRow = m_line / 32;
        u8 attrOffset = attrRow * 8 + attrCol;
        u8 attrData = MemR(attrTableAddress + attrOffset);
        bool attrRight = ((x / 16) % 2) == 1;
        bool attrBottom = ((m_line / 16) % 2) == 1;
        u8 attrMaskShift = 0;
        if (attrRight)  attrMaskShift += 2;
        if (attrBottom) attrMaskShift += 4;
        u8 numPalette = (attrData & (0x03 << attrMaskShift)) >> attrMaskShift;
        u16 paletteAddress = 0x3F01 + (numPalette * 4);

        u16 tilePatternAddress = BgPatternTableAddress + (tileID * 16);

        u8 tileY = m_line - (tileRow * 8);
        // Cada linea se representa con 2 bytes (dos bit planes)
        u8 bitPlane0 = MemR(tilePatternAddress + tileY);
        u8 bitPlane1 = MemR(tilePatternAddress + tileY + 8);

        u8 tileX = x - (tileCol * 8);
        tileX = ABS(tileX - 7);
        u8 mask = (0x01 << tileX);
        u8 indexColor = (((bitPlane1 & mask) << 1) | (bitPlane0 & mask)) >> tileX;

        u16 colorAddress = (indexColor == 0) ? 0x3F00 : (paletteAddress + (indexColor-1));
        u8  colorData = MemR(colorAddress) & 0x3F;
        u8 r = PALETTE_2C02_NESTOPIA[colorData * 3 + 0];
        u8 g = PALETTE_2C02_NESTOPIA[colorData * 3 + 1];
        u8 b = PALETTE_2C02_NESTOPIA[colorData * 3 + 2];

        m_screen->OnDrawPixel(r, g, b, x, m_line);
    }

    m_x = maxX;
}

u8 Video::MemR(u16 address) {
    if (address < 0x2000) // Pattern table 0 y 1
        return m_cartridge->ReadCHR(address);
    else if (address < 0x3000) // internal VRAM: Nametable 0, 1, 2, 3
        return m_VRAM[address - 0x2000];
    else if (address < 0x3F00) // Mirror
        return m_VRAM[address - 0x3000];
    else if (address < 0x3F20) { // Palette
        // Estas direcciones son mirrors
        if ((address == 0x3F10) || (address == 0x3F14) || (address == 0x3F18) || (address == 0x3F1C))
            address -= 0x10;
        return m_palette[address - 0x3F00];
    }
    else if (address < 0x3FFF) // Mirror
        return m_palette[(address - 0x3F20) % 0x0020];
    else
        return 0;
}

void Video::MemW(u16 address, u8 value) {
    if (address < 0x2000) // Pattern table 0 y 1
        m_cartridge->WriteCHR(address, value);
    else if (address < 0x3000) // internal VRAM: Nametable 0, 1, 2, 3
        m_VRAM[address - 0x2000] = value;
    else if (address < 0x3F00) // Mirror
        m_VRAM[address - 0x3000] = value;
    else if (address < 0x3F20) { // Palette
        if ((address == 0x3F10) || (address == 0x3F14) || (address == 0x3F18) || (address == 0x3F1C))
            address -= 0x10;
        m_palette[address - 0x3F00] = value;
    }
    else if (address < 0x3FFF) // Mirror
        m_palette[(address - 0x3F20) % 0x0020] = value;
    else
        return;
}

u16 Video::GetLine() {
    return m_line;
}

u8 Video::GetCyclesLine() {
    return m_cyclesLine;
}

void Video::GetTile(u8* buffer, int widthSize, int tile) {
    int addressTile = tile * 16;

    for (int y = 0; y < 8; y++)
    {
        int addressLineTile = addressTile + y;
        // Cada linea se representa con 2 bytes (dos bit planes)
        u8 bitPlane0 = MemR(addressLineTile + 0);
        u8 bitPlane1 = MemR(addressLineTile + 8);

        for (int x = 0; x < 8; x++)
        {
            int pixX = ABS(x - 7);
            u8 mask = (0x01 << pixX);
            u8 indexColor = (((bitPlane1 & mask)<<1) | (bitPlane0 & mask)) >> pixX;

            int offset = widthSize * y + x * 3;

            buffer[offset + 0] = indexColor * 85;
            buffer[offset + 1] = indexColor * 85;
            buffer[offset + 2] = indexColor * 85;
        }
    }
}

void Video::SaveState(ostream *stream) {
    
}

void Video::LoadState(istream *stream) {
    
}
