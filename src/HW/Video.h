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

#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "map"
#include "Def.h"

#define VDP_MEM 0x4000

enum t_VDPMODES {
    GRAPHIC_1, GRAPHIC_2, MODE_12, MODE_13, MODE_23, MODE_123,
    MULTICOLOR,
    MODE_4_192, MODE_4_224, MODE_4_240,
    TEXT, INVALID_TEXT
};

class Memory;
class ISMSScreenDrawable;

struct VideoPixel
{
	int x, y;
	int rowMap;
	int color, indexColor, xScrolled;
	int palette[4];
	int mapIni;
	u8 yTile;
    u8 r, g, b;
};

class Video
{
public:
	Video(ISMSScreenDrawable *screen);
	~Video(void);
    void SetScreen(ISMSScreenDrawable *screen);
    void Reset();
	void RefreshScreen();
	void ClearScreen();
    bool Interrupt();
    
    u8   GetV();                //0x7E
    u8   GetH();                //0x7F
    u8   GetData();             //0xBE
    void SetData(u8 value);     //0xBE
    u8   GetControl();          //0xBF
    void SetControl(u8 value);  //0xBF
    
    u8   MemR(u16 address);
    u8   RegR(u8 reg);
    u8   PalR(u8 pal);
    void GetTile(u8 *buffer, int widthSize, int tile);
    
    void Update(u8 cycles);
    
    u16  GetLine();
    u8   GetCyclesLine();
    u8   GetStatus();
    bool GetIE0();
    bool GetIE1();
    
private:
	u8  m_memory[VDP_MEM];
    u8  m_regs[16];
    u8  m_palettes[32];
    u8  m_rgbPalettes[32][3];
    u16 m_address;
    u8  m_numWrite;
    u8  m_partialAddress;
    u16 m_line;
    u32 m_cycles;
    u16 m_cyclesLine;
    u8  m_status;
    s16 m_lineIrqCounter;
    bool m_vramAddress;
	ISMSScreenDrawable *m_screen;
	VideoPixel *m_pixel;
    bool m_pendingVIRQ;
    bool m_pendingLIRQ;
    t_VDPMODES m_mode;
    bool m_priorityBG[SMS_SCREEN_W][SMS_SCREEN_H];

    void UpdateLine(u8 line);
	void UpdateBG(u8 y);
	void UpdateSprites(u8 y);
	inline void GetColor(VideoPixel * p);
    void UpdatePalette(u8 numPalette);
    void CheckReg(u8 reg);
};

#endif
