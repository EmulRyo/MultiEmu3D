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

#ifndef __NES_VIDEO_H__
#define __NES_VIDEO_H__

#include <map>
#include <iostream>
#include "Def.h"
#include "Cartridge.h"

class IScreenDrawable;

namespace Nes {

    class Video
    {
    public:
        Video(IScreenDrawable *screen);
        ~Video(void);
        void SetScreen(IScreenDrawable *screen);
        void SetCartridge(Cartridge* c);
        void Reset();
        void RefreshScreen();
        void ClearScreen();
        bool NMI();
        
        void WriteReg(u16 address, u8 value);
        u8   ReadReg(u16 address, bool debug=false);
        void GetTile(u8 *buffer, int widthSize, int tile);

        u8   MemR(u16 address);
        u8   MemRInternal(u16 address);
        void MemW(u16 address, u8 value);
        u8   OAMR(u16 address) const;
        
        void Update(u16 cycles);

        u32 GetNumFrames() const;
        u16 GetX() const;
        u16 GetY() const;
        u8  GetScrollX() const;
        u8  GetScrollY() const;
        u16 GetCurrentAddress() const;
        u16 GetTempAddress() const;
        u8  GetFineXScroll() const;
        u8  GetWriteToggle() const;
        
        void SaveState(std::ostream *file) const;
        void LoadState(std::istream *file);
        
    private:
        struct BGPixel {
            // Input
            u16 x;
            u16 patternTableAddress;
            bool show8Left;

            // Output
            bool valid;
            u8 colorId;
            u8 r, g, b;
        };
        struct SpritePixel {
            // Input
            u16 xScreen;
            u16 patternTableAddress;
            bool size16;
            bool show8Left;

            // Output
            bool valid;
            u8 id;
            u8 priorityBg;
            u8 colorId;
            u8 r, g, b;
        };
        struct BGTileCache {
            u8 bitPlane0;
            u8 bitPlane1;
            u8 numPalette;
        };

        u8 m_regs[8];
        u8 m_VRAM[0x1000];
        u8 m_readBuffer;
        u8 m_palette[0x20];
        u8  m_OAM[256];
        u8  m_OAMAddress;
        u8  m_secondaryOAM[64];
        u8  m_secondaryOAMLength;
        u16 m_secondaryOAMLine;
        Cartridge* m_cartridge;
        u16 m_nextDot;
        u32 m_cycles;
        u32 m_numFrames;
        IScreenDrawable *m_screen;
        u8  m_genLatch;
        u32 m_genLatchDecayCycles;
        bool m_NMI;
        u8 m_NMIDelay;
        BGTileCache m_bgTileCache[33];
        u16 m_bgTileCacheLine;
        u16 m_bgTileCacheT;
        u8  m_bgTileCacheX;
        u16 m_bgTileCachePatternTable;
        u16 m_bgTileCacheBaseLine;
        u16 m_scrollBaseLine;

        // PPU internal registers
        // https://wiki.nesdev.com/w/index.php?title=PPU_scrolling
        // yyy NN YYYYY XXXXX
        // ||| || ||||| +++++-- coarse X scroll
        // ||| || +++++-------- coarse Y scroll
        // ||| ++-------------- nametable select
        // +++----------------- fine Y scroll
        // ----------------------
        u16 m_v; // current VRAM address
        u16 m_t; // temp VRAM address. Address of the top left tile (15 bits)
        u8  m_x; // fine x scroll (3 bits)
        u8  m_w; // write toggle (1 bit)

        u8 VRAMR(u16 address) const;
        void OnEndFrame();
        u16  GetBGPaletteAddress(u16 x, u16 y, u16 attrTableAddress);
        void SpriteEvaluation(u16 line);
        void DrawPixels();
        void BuildBGLineCache(u16 line, u16 patternTableAddress);
        void PixelBG(BGPixel& bgPix);
        void PixelSprite(SpritePixel& sprPix);
        bool SpriteZeroHit(u16 x, u16 line, const SpritePixel& sprPix);
        void CoarseXIncrement(u16& v);
        void CoarseYIncrement(u16& v);
        void FineYIncrement(u16& v);
        void ScanlineEvents(u16 prevDot, u16 dot, u16 line);
    };
}

#endif
