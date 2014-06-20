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

class Memory;
class ISMSScreenDrawable;

struct VideoPixel
{
	int x, y;
	int rowMap, tileDataSelect;
	int color, indexColor, xScrolled;
	int palette[4];
	int mapIni;
	BYTE yTile;
    BYTE r, g, b;
};

class Video
{
private:
	Memory *m_mem;
	ISMSScreenDrawable *m_screen;
	VideoPixel *m_pixel;
public:
	Video(ISMSScreenDrawable * screen);
	~Video(void);
    void SetScreen(ISMSScreenDrawable * screen);
	void SetMem(Memory *mem);
	void RefreshScreen();
	void ClearScreen();
	void UpdateLine(BYTE line);
    void GetColorPalette(BYTE palette[4][3], int address);
private:
	void UpdateBG(int line);
	void UpdateOAM(int line);
	inline void GetColor(VideoPixel * p);
};

#endif
