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
#include <assert.h>
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
    m_screen->OnRefreshEmulatedScreen();
}

void Video::Reset() {
    m_nextDot = 0;
    m_cycles = 0;
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
    m_v = 0x0000;
    m_t = 0x0000;
    m_w = 0;
    m_x = 0;

    m_readBuffer = 0;
	for (int i = 0; i < 0x20; i++)
        m_palette[i] = 0;
    m_OAMAddress = 0;
    m_NMI = false;
    m_NMIDelay = 0;
    m_genLatchDecayCycles = 0;
    m_bgTileCacheLine = 0xFFFF;
    m_bgTileCacheT = 0xFFFF;
    m_bgTileCacheX = 0xFF;
    m_bgTileCachePatternTable = 0xFFFF;

    memset(m_OAM, 0xFF, 256);
    m_secondaryOAMLength = 0;
    m_secondaryOAMLine = 0xFFFF;

    memset(m_VRAM, 0, 0x1000);
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
        m_NMI = false;
        m_NMIDelay = 0;
        m_w = 0; // Reset write toggle
        m_genLatch = value;
        m_genLatchDecayCycles = 0;
        return value;
    }
    else if (address == OAMDATA) {
        u16 line = GetY();
        u16 dot = GetX();
        bool renderingEnabled = (m_regs[PPUMASK & 0x07] & 0x18) != 0;
        bool renderingLine = (line < 240) || (line == 261);
        u8 value = m_OAM[m_OAMAddress];
        if (renderingEnabled && renderingLine) {
            if ((dot >= 1 && dot <= 64) || (dot >= 257 && dot <= 320))
                value = 0xFF;
            else if (dot >= 65 && dot <= 256)
                value = m_OAM[((dot - 65) / 2) & 0xFF];
        }
        else if ((m_OAMAddress & 0x03) == 0x02)
            value &= 0xE3;
        m_genLatch = value;
        m_genLatchDecayCycles = 0;
        return value;
    }
    else if (address == PPUDATA) {
        u8 value = MemR(m_v);
        u8 increment = (m_regs[PPUCTRL & 0x07] & 0x04) == 0 ? 1 : 32;
        m_v = (m_v + increment) & 0x3FFF;

        m_genLatch = value;
        m_genLatchDecayCycles = 0;
        return value;
    }
    else
        return m_genLatch;
}

void Video::WriteReg(u16 address, u8 value) {
    m_genLatch = value;
    m_genLatchDecayCycles = 0;
    address = ((address - 0x2000) % 8) + 0x2000;
    if (address == PPUCTRL) {
        bool nmiWasEnabled = (m_regs[PPUCTRL & 0x07] & 0x80) != 0;
        bool nmiEnabled = (value & 0x80) != 0;
        if (!nmiWasEnabled && nmiEnabled && (m_regs[PPUSTATUS & 0x07] & 0x80)) {
            m_NMI = true;
            m_NMIDelay = 1;
        }
        else if (!nmiEnabled) {
            m_NMI = false;
            m_NMIDelay = 0;
        }

        m_t = (m_t & 0xF3FF) | ((value & 0x03) << 10);
    }
    else if (address == OAMADDR) {
        m_OAMAddress = value;
        m_regs[OAMDATA & 0x07] = m_OAM[m_OAMAddress];
    }
    else if (address == OAMDATA) {
        bool renderingEnabled = (m_regs[PPUMASK & 0x07] & 0x18) != 0;
        u16 line = GetY();
        if (renderingEnabled && (line < 240 || line == 261))
            m_OAMAddress = (m_OAMAddress + 4) & 0xFC;
        else {
            m_OAM[m_OAMAddress] = value;
            m_OAMAddress++;
        }
    }
    else if (address == PPUSCROLL) {
        if (m_w == 0) { // X
            m_t = (m_t & 0x7FE0) | (value >> 3);
            m_x = value & 0x07;
        }
        else {
            m_t = (m_t & 0xC1F) | ((value & 0x07) << 12) | ((value & 0xF8) << 2);
        }
        m_w = (m_w + 1) % 2;
    }
    else if (address == PPUADDR) {
        if (m_w == 0) { // Primera escritura, upper byte
            m_t = (m_t & 0x00FF) | ((value & 0x3F) << 8);
        }
        else { // Segunda escritura, lower byte
            m_t = (m_t & 0xFF00) | value;
            m_v = m_t;
        }
        m_w = (m_w + 1) % 2;
    }
    else if (address == PPUDATA) {
        MemW(m_v, value);
        u8 increment = (m_regs[PPUCTRL & 0x07] & 0x04) == 0 ? 1 : 32;
        m_v = (m_v + increment) & 0x3FFF;
    }
    
    if (address != PPUSTATUS) {
        u8 regID = address & 0x07;
        m_regs[regID] = value;
    }
}

void Video::Update(u16 cpuCycles) {
    u16 ppuCycles = cpuCycles * 3;
	m_genLatchDecayCycles += ppuCycles;
	if (m_genLatchDecayCycles > (NES_FRAME_PPU_CYCLES * 5))
		m_genLatch = 0;

    while (ppuCycles > 0) {
        u16 prevDot = m_cycles % NES_SCANLINE_PPU_CYCLES;

        u16 cycles = ppuCycles;
        if ((prevDot + ppuCycles) >= NES_SCANLINE_PPU_CYCLES)
            cycles = NES_SCANLINE_PPU_CYCLES - prevDot - 1;

        m_cycles += cycles;
        ppuCycles -= cycles;

        u16 line = m_cycles / NES_SCANLINE_PPU_CYCLES;
        u16 dot = m_cycles % NES_SCANLINE_PPU_CYCLES;

        DrawPixels();

        ScanlineEvents(prevDot, dot, line);

        if (dot >= NES_SCANLINE_PPU_CYCLES-1) {
            if (ppuCycles > 0) {
                m_cycles += 1;
                ppuCycles -= 1;
            }
            m_nextDot = 0;
            line = m_cycles / NES_SCANLINE_PPU_CYCLES;
            if (line == NES_SCREEN_H)
                RefreshScreen();
            else if (line == 261) { // Pre-render line
                // Clear Sprite Overflow, Sprite 0 Hit and VBlank
                m_regs[PPUSTATUS & 0x07] = (m_regs[PPUSTATUS & 0x07] & 0x1F);
            }
            else if (line == 262)
                OnEndFrame();
        }
    }
}

void Video::ScanlineEvents(u16 prevDot, u16 dot, u16 line) {
    bool renderingEnabled = ((m_regs[PPUMASK & 0x07] & 0x18) > 0);
    bool scanlineWithEvents = (line < 240) || (line == 261);

    if (renderingEnabled && scanlineWithEvents) {
        for (int i = prevDot + 1; i <= dot; i++) {
            if (((i <= 256) || (i >= 328)) && (i % 8 == 0)) {
                CoarseXIncrement(m_v);
            }
        }
        if ((prevDot < 256) && (dot >= 256)) {
            FineYIncrement(m_v);
        }
        if ((prevDot < 257) && (dot >= 257)) {
            m_v = (m_v & 0x7BE0) | (m_t & 0x041F);
            m_OAMAddress = 0;
        }
        if ((line == 261) && (prevDot >= 280) && (dot <= 304)) { // end of vblank
            m_v = (m_v & 0x041F) | (m_t & 0x7BE0);
        }

        // Sprite evaluation for the NEXT scanline (happens during dots 65-256)
        if ((prevDot < 65) && (dot >= 65)) {
            SpriteEvaluation((line == 261) ? 0 : line + 1);
        }

        // MMC3
        if (BIT3(m_regs[PPUCTRL & 0x07])) { // SpritePattern address
            if ((prevDot < 260) && (dot >= 260)) {
                m_cartridge->Scanline();
            }
        }
        else {
            if ((prevDot < 324) && (dot >= 324)) {
                m_cartridge->Scanline();
            }
        }
    }

    // VBlank flag set at dot 1 of scanline 241
    if ((line == 241) && (prevDot < 1) && (dot >= 1)) {
        u8 regID = PPUSTATUS & 0x07;
        m_regs[regID] |= 0x80;
        u8 ppuCtrlData = m_regs[PPUCTRL & 0x07];
        if ((ppuCtrlData & 0x80) > 0) {
            m_NMI = true;
        }
    }

    // VBlank flag cleared at dot 1 of pre-render scanline 261
    if ((line == 261) && (prevDot < 1) && (dot >= 1)) {
        m_regs[PPUSTATUS & 0x07] = (m_regs[PPUSTATUS & 0x07] & 0x1F);
    }
}

bool Video::NMI() {
    if (m_NMI) {
        if (m_NMIDelay > 0) {
            m_NMIDelay--;
            return false;
        }
        m_NMI = false;
        return true;
    } else
        return false;
}

void Video::OnEndFrame() {
    m_cycles = m_cycles % NES_FRAME_PPU_CYCLES;
    m_numFrames++;
}

void Video::SpriteEvaluation(u16 line) {
    u8 ppuCtrl = m_regs[PPUCTRL & 0x07];
    u8 size = (ppuCtrl & 0x20) > 0 ? 16 : 8;
    // Initialize secondary OAM to $FF as per PPU spec
    memset(m_secondaryOAM, 0xFF, 64);
    m_secondaryOAMLength = 0;
    m_secondaryOAMLine = line;
    for (u8 i = 0; i < 64; i++) {
        u8 y = m_OAM[i * 4];
        // Los sprites se pintan en y+1
        if (y < 255)
            y++;
        if ((y < 241) && (line >= y) && (line < (y+size))) {
            // Si se ha llegado al limite de 8 sprites
            if (m_secondaryOAMLength == 8) {
                if (BIT3(m_regs[PPUMASK & 0x07]) || BIT4(m_regs[PPUMASK & 0x07]))
                    m_regs[PPUSTATUS & 0x07] = m_regs[PPUSTATUS & 0x07] | 0x20;
                break;
            }
            m_secondaryOAM[m_secondaryOAMLength] = i;
            m_secondaryOAMLength++;
        }
    }
}

void Video::DrawPixels() {
    u16 line = m_cycles / NES_SCANLINE_PPU_CYCLES;
    if (line >= NES_SCREEN_H)
        return;

    u8 ppuCtrl = m_regs[PPUCTRL & 0x07];
    u8 ppuMask = m_regs[PPUMASK & 0x07];

    BGPixel bgPix{};
    bgPix.patternTableAddress = (ppuCtrl & 0x10) > 0 ? 0x1000 : 0x0000;
    bgPix.show8Left = BIT1(ppuMask) ? true : false;
    
    SpritePixel sprPix{};
    sprPix.patternTableAddress = (ppuCtrl & 0x08) > 0 ? 0x1000 : 0x0000;
    sprPix.show8Left = BIT2(ppuMask) ? true : false;
    sprPix.size16 = (ppuCtrl & 0x20) > 0 ? true : false;

    if (m_secondaryOAMLine != line)
        SpriteEvaluation(line);

    if (BIT3(ppuMask))
        BuildBGLineCache(line, bgPix.patternTableAddress);

    u16 dot = m_cycles % NES_SCANLINE_PPU_CYCLES;
    u16 maxX = dot - 1;
    if (maxX > NES_SCREEN_W)
        maxX = NES_SCREEN_W;

    for (u16 x = m_nextDot; x < maxX; x++) {
        bgPix.x = sprPix.xScreen = x;
        bgPix.valid = sprPix.valid = false;
        
        if (BIT3(ppuMask))
            PixelBG(bgPix);

        if (BIT4(ppuMask))
            PixelSprite(sprPix);

        // Sprite 0 hit
        if ((bgPix.valid) && (bgPix.colorId > 0) && (sprPix.valid) && (sprPix.colorId > 0) && (sprPix.id == 0) && (x < 255)) {
            u8 ppuMask = m_regs[PPUMASK & 0x07];
            bool bgEnabled = BIT3(ppuMask);
            bool sprEnabled = BIT4(ppuMask);
            bool show8Left = BIT2(ppuMask);
            if (bgEnabled && sprEnabled && (show8Left || (x >= 8)))
                m_regs[PPUSTATUS & 0x07] |= 0x40;
        }

        if ((sprPix.valid) && ((sprPix.priorityBg == 0) || (!bgPix.valid) || ((sprPix.priorityBg > 0) && (bgPix.colorId == 0))))
            m_screen->OnDrawPixel(sprPix.r, sprPix.g, sprPix.b, x, line);
        else if (bgPix.valid)
            m_screen->OnDrawPixel(bgPix.r, bgPix.g, bgPix.b, x, line);
        else
            m_screen->OnDrawPixel(0, 0, 0, x, line);
    }

    m_nextDot = maxX;
}

void Video::BuildBGLineCache(u16 line, u16 patternTableAddress) {
    if (m_bgTileCacheLine == line && m_bgTileCacheT == m_t && m_bgTileCacheX == m_x && m_bgTileCachePatternTable == patternTableAddress)
        return;

    m_bgTileCacheLine = line;
    m_bgTileCacheT = m_t;
    m_bgTileCacheX = m_x;
    m_bgTileCachePatternTable = patternTableAddress;

    u16 fetchAddress = m_t;
    u16 fineY = (fetchAddress >> 12) & 0x07;
    u16 coarseYSteps = (fineY + line) / 8;
    fetchAddress = (fetchAddress & ~0x7000) | (((fineY + line) & 0x07) << 12);
    for (u16 i = 0; i < coarseYSteps; i++)
        CoarseYIncrement(fetchAddress);

    u16 fineXOffset = m_x;
    for (u16 i = 0; i < (fineXOffset / 8); i++)
        CoarseXIncrement(fetchAddress);

    u8 tileY = (fetchAddress >> 12) & 0x07;
    for (u8 i = 0; i < 33; i++) {
        u16 nameTableAddress = 0x2000 | (fetchAddress & 0x0FFF);
        u8 tileID = VRAMR(nameTableAddress);
        u16 tilePatternAddr = patternTableAddress + (tileID * 16);

        u16 attrAddress = 0x23C0 | (fetchAddress & 0x0C00) | ((fetchAddress >> 4) & 0x38) | ((fetchAddress >> 2) & 0x07);
        u8 attrData = MemRInternal(attrAddress);
        u8 attrMaskShift = ((fetchAddress & 0x0002) ? 2 : 0) | ((fetchAddress & 0x0040) ? 4 : 0);

        m_bgTileCache[i].bitPlane0 = MemRInternal(tilePatternAddr + tileY);
        m_bgTileCache[i].bitPlane1 = MemRInternal(tilePatternAddr + tileY + 8);
        m_bgTileCache[i].numPalette = (attrData >> attrMaskShift) & 0x03;

        CoarseXIncrement(fetchAddress);
    }
}

void Video::PixelBG(BGPixel& bgPix) {
    if ((bgPix.x < 8) && (!bgPix.show8Left))
        return;

    u16 fineXOffset = m_x + bgPix.x;
    u16 tileIndex = fineXOffset / 8;
    u8 tileX = fineXOffset & 0x07;
    BGTileCache& tile = m_bgTileCache[tileIndex];

    // Extract pixel (bit 7 = leftmost pixel)
    u8 bitPos = 7 - tileX;
    u8 mask = (0x01 << bitPos);
    u8 colorId = (((tile.bitPlane1 & mask) << 1) | (tile.bitPlane0 & mask)) >> bitPos;

    u16 paletteAddr = 0x3F01 + (tile.numPalette * 4);
    u16 colorAddress = (colorId == 0) ? 0x3F00 : (paletteAddr + (colorId - 1));
    u8 colorData = MemRInternal(colorAddress) & 0x3F;

    bgPix.valid = true;
    bgPix.colorId = colorId;
    bgPix.r = PALETTE_2C02_NESTOPIA[colorData * 3 + 0];
    bgPix.g = PALETTE_2C02_NESTOPIA[colorData * 3 + 1];
    bgPix.b = PALETTE_2C02_NESTOPIA[colorData * 3 + 2];
}

void Video::PixelSprite(SpritePixel& sprPix) {
    sprPix.valid = false;

    if ((sprPix.xScreen < 8) && (!sprPix.show8Left))
        return;

    u16 line = m_cycles / NES_SCANLINE_PPU_CYCLES;

    for (u8 s = 0; s < m_secondaryOAMLength; s++) {
        u8 id = m_secondaryOAM[s];
        u16 xStart = m_OAM[id * 4 + 3];
        if ((sprPix.xScreen >= xStart) && (sprPix.xScreen < (xStart + 8))) {
            u16 yStart = m_OAM[id * 4 + 0] + 1; // Los sprites se pintan en y+1
            u16 spriteHeight = sprPix.size16 ? 16 : 8;
            if ((line < yStart) || (line >= (yStart + spriteHeight)))
                continue;

            u8 tileID = m_OAM[id * 4 + 1];
            u8 attr = m_OAM[id * 4 + 2];
            u8 flipX = BIT6(attr);
            u8 flipY = BIT7(attr);
            u8 row = line - yStart;
            u8 fineX = sprPix.xScreen - xStart;
            u16 patternTableAddress = sprPix.patternTableAddress;

            if (flipY)
                row = spriteHeight - 1 - row;

            if (sprPix.size16) {
                patternTableAddress = (tileID & 1) ? 0x1000 : 0x0000;
                tileID = (tileID & 0xFE) + (row / 8);
            }
            u8 fineY = row % 8;
                 
            u16 tilePatternAddress = patternTableAddress + (tileID * 16);
            u8 bitPlane0 = MemRInternal(tilePatternAddress + fineY);
            u8 bitPlane1 = MemRInternal(tilePatternAddress + fineY + 8);
            if (flipX == 0)
                fineX = ABS(fineX - 7);
            u8 mask = (0x01 << fineX);
            u8 colorId = (((bitPlane1 & mask) << 1) | (bitPlane0 & mask)) >> fineX;
            if (colorId > 0) {
                u8  numPalette = attr & 0x03;
                u16 paletteAddress = 0x3F11 + (numPalette * 4);
                u16 colorAddress = paletteAddress + (colorId - 1);
                u8  colorData = MemRInternal(colorAddress) & 0x3F;

                sprPix.valid = true;
                sprPix.id = id;
                sprPix.colorId = colorId;
                sprPix.priorityBg = BIT5(attr) >> 5;
                sprPix.r = PALETTE_2C02_NESTOPIA[colorData * 3 + 0];
                sprPix.g = PALETTE_2C02_NESTOPIA[colorData * 3 + 1];
                sprPix.b = PALETTE_2C02_NESTOPIA[colorData * 3 + 2];
                break; // Stop at first visible sprite (highest priority)
            }
        }
    }
}

u8 Video::MemR(u16 address) {
    address &= 0x3FFF;
    if (address < 0x3000) { // PPUDATA read buffer (post-fetch)
        u8 value = m_readBuffer;
        if (address < 0x2000) // Pattern table 0 y 1
            value = m_cartridge->ReadCHR(address);
        else // internal VRAM: Nametable 0, 1, 2, 3
            value = VRAMR(address);

        u8 retValue = m_readBuffer;
        m_readBuffer = value;
        return retValue;
    }
    else if (address < 0x3F00) // Mirror
        return MemR(address - 0x3000);
    else if (address < 0x3F20) { // Palette
        m_readBuffer = MemRInternal(address - 0x1000); // El buffer se rellena con el mirror del nametable si no existiera la paleta
        // Estas direcciones son mirrors
        if ((address == 0x3F10) || (address == 0x3F14) || (address == 0x3F18) || (address == 0x3F1C))
            address -= 0x10;
        u8 value = m_palette[address - 0x3F00] & 0x3F;
        if (m_regs[PPUMASK & 0x07] & 0x01)
            value &= 0x30;
        return value | (m_genLatch & 0xC0);
    }
    else if (address < 0x4000) { // Mirror
        address = 0x3F00 | (address & 0x001F);
        m_readBuffer = MemRInternal(address - 0x1000);  // El buffer se rellena con el mirror del nametable si no existiera la paleta
        if ((address == 0x3F10) || (address == 0x3F14) || (address == 0x3F18) || (address == 0x3F1C))
            address -= 0x10;
        u8 value = m_palette[address - 0x3F00] & 0x3F;
        if (m_regs[PPUMASK & 0x07] & 0x01)
            value &= 0x30;
        return value | (m_genLatch & 0xC0);
    }
    else
        return 0;
}

u8 Video::MemRInternal(u16 address) {
    address &= 0x3FFF;
    if (address < 0x3000) { // PPUDATA read buffer (post-fetch)
        u8 value = m_readBuffer;
        if (address < 0x2000) // Pattern table 0 y 1
            value = m_cartridge->ReadCHR(address);
        else // internal VRAM: Nametable 0, 1, 2, 3
            value = VRAMR(address);

        return value;
    }
    else if (address < 0x3F00) // Mirror
        return MemRInternal(address - 0x3000);
    else if (address < 0x3F20) { // Palette
        // Estas direcciones son mirrors
        if ((address == 0x3F10) || (address == 0x3F14) || (address == 0x3F18) || (address == 0x3F1C))
            address -= 0x10;
        return m_palette[address - 0x3F00] & 0x3F;
    }
    else if (address < 0x4000) { // Mirror
        address = 0x3F00 | (address & 0x001F);
        if ((address == 0x3F10) || (address == 0x3F14) || (address == 0x3F18) || (address == 0x3F1C))
            address -= 0x10;
        return m_palette[address - 0x3F00] & 0x3F;
    }
    else
        return 0;
}

u8 Video::VRAMR(u16 address) const {
    if (m_cartridge->GetNametableMirroring() == NametableMirroring::VERTICAL) {
        return m_VRAM[(address - 0x2000) % 0x800];
    }
    else if (m_cartridge->GetNametableMirroring() == NametableMirroring::HORIZONTAL) {
        if (address < 0x2400)
            return m_VRAM[address - 0x2000];
        else if (address < 0x2800)
            return m_VRAM[address - 0x2400];
        else if (address < 0x2C00)
            return m_VRAM[address - 0x2000];
        else
            return m_VRAM[address - 0x2400];
    }
    else if (m_cartridge->GetNametableMirroring() == NametableMirroring::SINGLE_SCREEN_LOWER) {
        return m_VRAM[(address - 0x2000) & 0x03FF];
    }
    else if (m_cartridge->GetNametableMirroring() == NametableMirroring::SINGLE_SCREEN_UPPER) {
        return m_VRAM[0x0400 | ((address - 0x2000) & 0x03FF)];
    }
    else
        return m_VRAM[address - 0x2000];
}

void Video::MemW(u16 address, u8 value) {
    address &= 0x3FFF;
    if (address < 0x2000) // Pattern table 0 y 1
        m_cartridge->WriteCHR(address, value);
    else if (address < 0x3000) { // internal VRAM: Nametable 0, 1, 2, 3
        if (m_cartridge->GetNametableMirroring() == NametableMirroring::VERTICAL) {
            m_VRAM[(address - 0x2000) % 0x800] = value;
        }
        else if (m_cartridge->GetNametableMirroring() == NametableMirroring::HORIZONTAL) {
            if (address < 0x2400)
                m_VRAM[address - 0x2000] = value;
            else if (address < 0x2800)
                m_VRAM[address - 0x2400] = value;
            else if (address < 0x2C00)
                m_VRAM[address - 0x2000] = value;
            else
                m_VRAM[address - 0x2400] = value;
        }
        else if (m_cartridge->GetNametableMirroring() == NametableMirroring::SINGLE_SCREEN_LOWER) {
            m_VRAM[(address - 0x2000) & 0x03FF] = value;
        }
        else if (m_cartridge->GetNametableMirroring() == NametableMirroring::SINGLE_SCREEN_UPPER) {
            m_VRAM[0x0400 | ((address - 0x2000) & 0x03FF)] = value;
        }
        else
            m_VRAM[address - 0x2000] = value;
    }
    else if (address < 0x3F00) // Mirror
        MemW(address - 0x1000, value);
    else if (address < 0x3F20) { // Palette
        if ((address == 0x3F10) || (address == 0x3F14) || (address == 0x3F18) || (address == 0x3F1C))
            address -= 0x10;
        m_palette[address - 0x3F00] = value & 0x3F;
    }
    else if (address < 0x4000) // Mirror
    {
        address = 0x3F00 | (address & 0x001F);
        if ((address == 0x3F10) || (address == 0x3F14) || (address == 0x3F18) || (address == 0x3F1C))
            address -= 0x10;
        m_palette[address - 0x3F00] = value & 0x3F;
    }
    else
        return;
}

void Video::GetTile(u8* buffer, int widthSize, int tile) {
    int addressTile = tile * 16;

    for (int y = 0; y < 8; y++)
    {
        int addressLineTile = addressTile + y;
        // Cada linea se representa con 2 bytes (dos bit planes)
        u8 bitPlane0 = MemRInternal(addressLineTile + 0);
        u8 bitPlane1 = MemRInternal(addressLineTile + 8);

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

void Video::FineYIncrement(u16& v) {
    if ((v & 0x7000) != 0x7000)             // if fine Y < 7
        v += 0x1000;                        // increment fine Y
    else {
        v &= ~0x7000;                       // fine Y = 0
        int y = (v & 0x03E0) >> 5;          // let y = coarse Y
        if (y == 29) {
            y = 0;                          // coarse Y = 0
            v ^= 0x0800;                    // switch vertical nametable
        }
        else if (y == 31)
            y = 0;                          // coarse Y = 0, nametable not switched
        else
            y += 1;                         // increment coarse Y
        v = (v & ~0x03E0) | (y << 5);       // put coarse Y back into v
    }
}

void Video::CoarseYIncrement(u16& v) {
    int y = (v & 0x03E0) >> 5;
    if (y == 29) {
        y = 0;
        v ^= 0x0800;
    }
    else if (y == 31)
        y = 0;
    else
        y += 1;
    v = (v & ~0x03E0) | (y << 5);
}

void Video::CoarseXIncrement(u16& v) {
    if ((v & 0x001F) == 31) {   // if coarse X == 31
        v &= ~0x001F;           // coarse X = 0
        v ^= 0x0400;            // switch horizontal nametable
    }
    else
        v += 1;                 // increment coarse X
}

u32 Video::GetNumFrames() const {
    return m_numFrames;
}

u16 Video::GetX() const {
    return m_cycles % NES_SCANLINE_PPU_CYCLES;
}

u16 Video::GetY() const {
    return m_cycles / NES_SCANLINE_PPU_CYCLES;
}

u8 Video::GetScrollX() const {
    u8 coarseX = m_v & 0x1F;
    return (coarseX << 3) | m_x;
}

u8 Video::GetScrollY() const {
    u8 coarseY = (m_v & 0x3E0) >> 5;
    return (coarseY << 3) | ((m_v & 0x7000) >> 12);
}

u16 Video::GetCurrentAddress() const {
    return m_v;
}

u16 Video::GetTempAddress() const {
    return m_t;
}

u8 Video::GetFineXScroll() const {
    return m_x;
}

u8 Video::GetWriteToggle() const {
    return m_w;
}

void Video::SaveState(ostream *stream) const {
    stream->write((char*)&m_regs[0], sizeof(u8) * 8);
    stream->write((char*)&m_readBuffer, sizeof(u8));
    stream->write((char*)&m_palette[0], sizeof(u8) * 0x20);
    stream->write((char*)&m_v, sizeof(u16));
    stream->write((char*)&m_t, sizeof(u16));
    stream->write((char*)&m_w, sizeof(u8));
    stream->write((char*)&m_x, sizeof(u8));
    stream->write((char*)&m_OAM[0], sizeof(u8) * 256);
    stream->write((char*)&m_OAMAddress, sizeof(u8));
    stream->write((char*)&m_secondaryOAM[0], sizeof(u8) * 64);
    stream->write((char*)&m_secondaryOAMLength, sizeof(u8));
    stream->write((char*)&m_nextDot, sizeof(u16));
    stream->write((char*)&m_cycles, sizeof(u32));
    stream->write((char*)&m_numFrames, sizeof(u32));
    stream->write((char*)&m_genLatch, sizeof(u8));
    stream->write((char*)&m_NMI, sizeof(bool));
    stream->write((char*)&m_VRAM[0], sizeof(u8) * 0x1000);
}

void Video::LoadState(istream *stream) {
    stream->read((char*)&m_regs[0], sizeof(u8) * 8);
    stream->read((char*)&m_readBuffer, sizeof(u8));
    stream->read((char*)&m_palette[0], sizeof(u8) * 0x20);
    stream->read((char*)&m_v, sizeof(u16));
    stream->read((char*)&m_t, sizeof(u16));
    stream->read((char*)&m_w, sizeof(u8));
    stream->read((char*)&m_x, sizeof(u8));
    stream->read((char*)&m_OAM[0], sizeof(u8) * 256);
    stream->read((char*)&m_OAMAddress, sizeof(u8));
    stream->read((char*)&m_secondaryOAM[0], sizeof(u8) * 64);
    stream->read((char*)&m_secondaryOAMLength, sizeof(u8));
    stream->read((char*)&m_nextDot, sizeof(u16));
    stream->read((char*)&m_cycles, sizeof(u32));
    stream->read((char*)&m_numFrames, sizeof(u32));
    stream->read((char*)&m_genLatch, sizeof(u8));
    stream->read((char*)&m_NMI, sizeof(bool));
    stream->read((char*)&m_VRAM[0], sizeof(u8) * 0x1000);

    m_secondaryOAMLine = 0xFFFF;
    m_v &= 0x3FFF;
    m_t &= 0x7FFF;
    m_x &= 0x07;
    m_w &= 0x01;
}

u8 Video::OAMR(u16 address) const {
    return (address < 0x100) ? m_OAM[address] : 0;
}
