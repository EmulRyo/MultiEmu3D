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

class Memory;
class ISMSScreenDrawable;

struct VideoPixel
{
	int x, y;
	int rowMap, tileDataSelect;
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
	void RefreshScreen();
	void ClearScreen();
	void UpdateLine(u8 line);
    void GetColorPalette(u8 palette[4][3], int address);
    
    u8   GetAddress();          //0xBF
    void SetAddress(u8 value);  //0xBF
    
    u8   GetData();             //0xBE
    void SetData(u8 value);     //0xBE
    
private:
	u8  m_memory[VDP_MEM];
    u8  m_regs[16];
    u8  m_palettes[32];
    u16 m_address;
    u8  m_numWrite;
    u8  m_partialAddress;
    bool m_vramAddress;
	ISMSScreenDrawable *m_screen;
	VideoPixel *m_pixel;

	void UpdateBG(int line);
	void UpdateOAM(int line);
	inline void GetColor(VideoPixel * p);
};

#endif
