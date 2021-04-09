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
    Reset();
	SetScreen(screen);
}

Video::~Video(void)
{
	
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
    m_writeToggle = 0;

    memset(m_OAM, 0xFF, 256);
    m_secondaryOAMLength = 0;

    memset(m_VRAM, 0, 0x1000);

    m_scrollX = m_scrollY = 0;
    m_scrollXRequest = m_scrollYRequest = 0;
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
        m_writeToggle = 0;
        m_genLatch = m_regs[regID];
        return value;
    }
    else if (address == OAMDATA) {
        m_genLatch = m_regs[regID];
        return m_regs[regID];
    }
    else if (address == PPUDATA) {
        u8 value = MemR(m_addressLatch, false);
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
    if (address == OAMADDR) {
        m_OAMAddress = value;
        m_regs[OAMDATA & 0x07] = m_OAM[m_OAMAddress];
    }
    else if (address == OAMDATA) {
        m_OAM[m_OAMAddress] = value;
        m_OAMAddress++;
    }
    else if (address == PPUSCROLL) {
        if (m_writeToggle == 0) // X
            m_scrollXRequest = value;
        else
            m_scrollYRequest = value;
        m_writeToggle = (m_writeToggle + 1) % 2;
    }
    else if (address == PPUADDR) {
        if (m_writeToggle == 0) // Primera escritura, upper byte
            m_addressLatch = (value << 8) | (m_addressLatch & 0x00FF);
        else // Segunda escritura, lower byte
            m_addressLatch = (m_addressLatch & 0xFF00) | value;
        m_writeToggle = (m_writeToggle+1) % 2;
    }
    else if (address == PPUDATA) {
        MemW(m_addressLatch, value);
        u8 increment = (m_regs[PPUCTRL & 0x07] & 0x04) == 0 ? 1 : 32;
        m_addressLatch += increment;
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
        m_line++;
        m_cyclesLine -= scanLineCycles;
        m_scrollX = m_scrollXRequest;
        if (m_line == NES_SCREEN_H)
            RefreshScreen();
        else if (m_line == 241) {
            // Set VBlank Flag
            u8 regID = PPUSTATUS & 0x07;
            m_regs[regID] |= 0x80;

            u8 ppuCtrlData = m_regs[PPUCTRL & 0x07];
            if ((ppuCtrlData & 0x80) > 0)
                NMI = true; // Si el bit 7 de PPUCTRL está activo, generar una Non-Maskable Interrupt
        }
        else if (m_line == 261) { // Pre-render line
            // Clear Sprite Overflow, Sprite 0 Hit and VBlank
            m_regs[PPUSTATUS & 0x07] = (m_regs[PPUSTATUS & 0x07] & 0x1F);
            m_scrollY = m_scrollYRequest;
        }
        
        SpriteEvaluation(m_line);
    }

    DrawPixels();

    return NMI;
}

void Video::OnEndFrame() {
    m_cycles -= NES_FRAME_PPU_CYCLES;
    m_cyclesLine = 0;
    m_line = 0;
    m_numFrames++;
}

void Video::SpriteEvaluation(u16 line) {
    m_secondaryOAMLength = 0;
    for (u8 i = 0; i < 64; i++) {
        u8 y = m_OAM[i * 4];
        // Los sprites se pintan en y+1
        if (y < 255)
            y++;
        if ((y < 240) && (line >= y) && (line < (y+8))) {
            m_secondaryOAM[m_secondaryOAMLength] = i;
            m_secondaryOAMLength++;
        }
    }
    if (m_secondaryOAMLength > 8)
        m_regs[PPUSTATUS & 0x07] = m_regs[PPUSTATUS & 0x07] | 0x20;
}

void Video::DrawPixels() {
    if (m_line >= NES_SCREEN_H)
        return;

    BGInput bgIn;
    BGOutput bgOut;
    SpriteInput sprIn;
    SpriteOutput sprOut;
    u8 ppuCtrl = m_regs[PPUCTRL & 0x07];
    u8 ppuMask = m_regs[PPUMASK & 0x07];
    bgIn.patternTableAddress = (ppuCtrl & 0x10) > 0 ? 0x1000 : 0x0000;
    bgIn.nameTableAddress = ((ppuCtrl & 0x03) * 0x400) + 0x2000;
    bgIn.attrTableAddress = bgIn.nameTableAddress + 0x03C0;
    bgIn.mirroring = m_cartridge->GetNametableMirroring();
    sprIn.patternTableAddress = (ppuCtrl & 0x08) > 0 ? 0x1000 : 0x0000;
    sprIn.size16 = (ppuCtrl & 0x20) > 0 ? true : false;

    u16 maxX = m_cyclesLine - 1;
    if (maxX > NES_SCREEN_W)
        maxX = NES_SCREEN_W;

    for (u16 x = m_x; x < maxX; x++) {
        bgIn.x = sprIn.xScreen = x;
        bgOut.valid = sprOut.valid = false;
        
        if (BIT3(ppuMask))
            PixelBG(bgIn, bgOut);

        if (BIT4(ppuMask))
            PixelSprite(sprIn, sprOut);

        // Sprite 0 hit
        if ((bgOut.valid) && (bgOut.colorId > 0) && (sprOut.valid) && (sprOut.id == 0))
            m_regs[PPUSTATUS & 0x07] |= 0x40;

        if ((sprOut.valid) && ((sprOut.priorityBg == 0) || (!bgOut.valid) || ((sprOut.priorityBg > 0) && (bgOut.colorId == 0))))
            m_screen->OnDrawPixel(sprOut.r, sprOut.g, sprOut.b, x, m_line);
        else if (bgOut.valid)
            m_screen->OnDrawPixel(bgOut.r, bgOut.g, bgOut.b, x, m_line);
        else
            m_screen->OnDrawPixel(0, 0, 0, x, m_line);
    }

    m_x = maxX;
}

void Video::PixelBG(BGInput in, BGOutput& out) {
    u16 nameTableAddress = in.nameTableAddress;
    u16 attrTableAddress = in.attrTableAddress;
    u16 x = in.x + m_scrollX;
    if (x > 255) { // Si se sale de su nametable cambiar x y direccion
        x -= 256;
        // Sumar 0x400 y si se sale del nametable 0x2400 volver al 0x2000
        nameTableAddress = (((nameTableAddress - 0x2000) + 0x400) % 0x800) + 0x2000;
    }
    u16 y = m_line + m_scrollY;
    if (y > 239) {
        y -= 240;
        // Sumar 0x800 y si se sale del nametable 0x2800 volver al 0x2000
        nameTableAddress = (((nameTableAddress - 0x2000) + 0x800) % 0x1000) + 0x2000;
    }
    attrTableAddress = nameTableAddress + 0x03C0;

    u8 tileCol = x / 8;
    u8 tileRow = y / 8;
    u16 nameTableOffset = tileRow * 32 + tileCol;
    u8 tileID = MemR(nameTableAddress + nameTableOffset);
    u16 tilePatternAddress = in.patternTableAddress + (tileID * 16);

    u8 tileY = y - (tileRow * 8);
    // Cada linea se representa con 2 bytes (dos bit planes)
    u8 bitPlane0 = MemR(tilePatternAddress + tileY);
    u8 bitPlane1 = MemR(tilePatternAddress + tileY + 8);

    u8 tileX = x - (tileCol * 8);
    tileX = ABS(tileX - 7);
    u8 mask = (0x01 << tileX);
    u8 colorId = (((bitPlane1 & mask) << 1) | (bitPlane0 & mask)) >> tileX;

    u16 paletteAddress = GetBGPaletteAddress(x, y, attrTableAddress);
    u16 colorAddress = (colorId == 0) ? 0x3F00 : (paletteAddress + (colorId - 1));
    u8  colorData = MemR(colorAddress) & 0x3F;

    out.valid = true;
    out.colorId = colorId;
    out.r = PALETTE_2C02_NESTOPIA[colorData * 3 + 0];
    out.g = PALETTE_2C02_NESTOPIA[colorData * 3 + 1];
    out.b = PALETTE_2C02_NESTOPIA[colorData * 3 + 2];
}

void Video::PixelSprite(SpriteInput in, SpriteOutput& out) {
    out.valid = false;

    s8 s = m_secondaryOAMLength - 1;
    while (s >= 0) {
        u8 id = m_secondaryOAM[s];
        u8 xStart = m_OAM[id * 4 + 3];
        if ((xStart > in.xScreen - 8) && (xStart <= in.xScreen)) {
            u8 yStart = m_OAM[id * 4 + 0] + 1; // Los sprites se pintan en y+1
            u8 tileID = m_OAM[id * 4 + 1];
            u8 attr = m_OAM[id * 4 + 2];
            u8 fineY = m_line - yStart;
            u8 fineX = in.xScreen - xStart;
            u16 tilePatternAddress = in.patternTableAddress + (tileID * 16);
            u8 bitPlane0 = MemR(tilePatternAddress + fineY);
            u8 bitPlane1 = MemR(tilePatternAddress + fineY + 8);
            if ((attr & 0x40) == 0)
                fineX = ABS(fineX - 7);
            u8 mask = (0x01 << fineX);
            u8 colorId = (((bitPlane1 & mask) << 1) | (bitPlane0 & mask)) >> fineX;
            if (colorId > 0) {
                u8  numPalette = attr & 0x03;
                u16 paletteAddress = 0x3F11 + (numPalette * 4);
                u16 colorAddress = paletteAddress + (colorId - 1);
                u8  colorData = MemR(colorAddress) & 0x3F;

                out.valid = true;
                out.id = id;
                out.colorId = colorId;
                out.priorityBg = BIT5(attr) >> 5;
                out.r = PALETTE_2C02_NESTOPIA[colorData * 3 + 0];
                out.g = PALETTE_2C02_NESTOPIA[colorData * 3 + 1];
                out.b = PALETTE_2C02_NESTOPIA[colorData * 3 + 2];
            }
        }
        s--;
    }
}

u16  Video::GetBGPaletteAddress(u16 x, u16 y, u16 attrTableAddress) {
    // Attribute Table
    u8 attrCol = x / 32;
    u8 attrRow = y / 32;
    u8 attrOffset = attrRow * 8 + attrCol;
    u8 attrData = MemR(attrTableAddress + attrOffset);
    bool attrRight  = ((x / 16) % 2) == 1;
    bool attrBottom = ((y / 16) % 2) == 1;
    u8 attrMaskShift = 0;
    if (attrRight)  attrMaskShift += 2;
    if (attrBottom) attrMaskShift += 4;
    u8 numPalette = (attrData & (0x03 << attrMaskShift)) >> attrMaskShift;
    u16 paletteAddress = 0x3F01 + (numPalette * 4);

    return paletteAddress;
}

u8 Video::MemR(u16 address, bool skipBuffer) {
    if (address < 0x3000) { // PPUDATA read buffer (post-fetch)
        u8 value = m_readBuffer;
        if (address < 0x2000) { // Pattern table 0 y 1
            value = m_cartridge->ReadCHR(address);
        }
        else if (address < 0x3000) {// internal VRAM: Nametable 0, 1, 2, 3
            if (m_cartridge->GetNametableMirroring() == NametableMirroring::VERTICAL) {
                value = m_VRAM[(address - 0x2000) % 0x800];
            }
            else if (m_cartridge->GetNametableMirroring() == NametableMirroring::HORIZONTAL) {
                if (address < 0x2400)
                    value = m_VRAM[address - 0x2000];
                else if (address < 0x2800)
                    value = m_VRAM[address - 0x2400];
                else if (address < 0x2C00)
                    value = m_VRAM[address - 0x2000];
                else
                    value = m_VRAM[address - 0x2400];
            }
            else
                value = m_VRAM[address - 0x2000];
        }

        u8 retValue = skipBuffer ? value : m_readBuffer;
        m_readBuffer = value;
        return retValue;
    }
    else if (address < 0x3F00) // Mirror
        return MemR(address - 0x3000, skipBuffer);
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

u32 Video::GetNumFrames() {
    return m_numFrames;
}

u16 Video::GetX() {
    return m_cyclesLine;
}

u16 Video::GetY() {
    return m_line;
}

u8 Video::GetScrollX() {
    return m_scrollX;
}

u8 Video::GetScrollY() {
    return m_scrollY;
}

u16 Video::GetAddress() {
    return m_addressLatch;
}

void Video::SaveState(ostream *stream) {
    
}

void Video::LoadState(istream *stream) {
    
}
