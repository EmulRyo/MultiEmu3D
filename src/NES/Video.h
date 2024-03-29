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
        bool Interrupt();
        
        void WriteReg(u16 address, u8 value);
        u8   ReadReg(u16 address, bool debug=false);
        void GetTile(u8 *buffer, int widthSize, int tile);

        u8   MemR(u16 address, bool skipBuffer=true);
        void MemW(u16 address, u8 value);

        u8   OAMR(u16 address);
        
        bool Update(u16 cycles);

        u32 GetNumFrames();
        u16 GetX();
        u16 GetY();
        u8  GetScrollX();
        u8  GetScrollY();
        u16 GetCurrentAddress();
        u16 GetTempAddress();
        u8  GetFineXScroll();
        u8  GetWriteToggle();
        
        void SaveState(std::ostream *file);
        void LoadState(std::istream *file);
        
    private:
        struct BGInput {
            u16 x;
            u16 nameTableAddress;
            u16 patternTableAddress;
            u16 attrTableAddress;
            bool show8Left;
            NametableMirroring mirroring;
        };
        struct BGOutput {
            bool valid;
            u8 colorId;
            u8 r, g, b;
        };
        struct SpriteInput {
            u16 xScreen;
            u16 patternTableAddress;
            bool size16;
            bool show8Left;
        };
        struct SpriteOutput {
            bool valid;
            u8 id;
            u8 priorityBg;
            u8 colorId;
            u8 r, g, b;
        };

        u8 m_regs[8];
        u8 m_VRAM[0x1000];
        u8 m_readBuffer;
        u8 m_palette[0x20];
        u16 m_addressLatch;
        u8  m_writeToggle;
        u8  m_OAM[256];
        u8  m_OAMAddress;
        u8  m_secondaryOAM[64];
        u8  m_secondaryOAMLength;
        Cartridge* m_cartridge;
        u16 m_x;
        u16 m_line;
        float m_cycles;
        u16 m_cyclesLine;
        u32 m_numFrames;
        u8  m_scrollX;
        u8  m_scrollY;
        u8  m_scrollXRequest;
        u8  m_scrollYRequest;
        IScreenDrawable *m_screen;
        u8  m_genLatch;
        u16 m_nameTableAddress;

        // PPU internal registers
        // https://wiki.nesdev.com/w/index.php?title=PPU_scrolling
        // yyy NN YYYYY XXXXX
        // ||| || ||||| +++++-- coarse X scroll
        // ||| || +++++-------- coarse Y scroll
        // ||| ++-------------- nametable select
        // +++----------------- fine Y scroll
        // ----------------------
        // u16 m_v;
        // u16 m_t;
        // u8  m_x;
        // u8  m_w;

        void OnEndFrame();
        u16  GetBGPaletteAddress(u16 x, u16 y, u16 attrTableAddress);
        void SpriteEvaluation(u16 line);
        void DrawPixels();
        void PixelBG(BGInput input, BGOutput& output);
        void PixelSprite(SpriteInput input, SpriteOutput& output);
    };
}

#endif
