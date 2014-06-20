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

#include <iostream>
#include "Memory.h"
#include "ISMSScreenDrawable.h"
#include "Video.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MEMR(address) (m_mem->memory[(address)])

using namespace std;

Video::Video(ISMSScreenDrawable *screen)
{
    m_pixel = new VideoPixel();
	SetScreen(screen);
}

Video::~Video(void)
{
	delete m_pixel;
}

void Video::SetScreen(ISMSScreenDrawable *screen)
{
    m_screen = screen;
}

void Video::SetMem(Memory *mem)
{
	m_mem = mem;
}

void Video::UpdateLine(BYTE y)
{
	UpdateBG(y);
	UpdateOAM(y);
}

void Video::RefreshScreen()
{
    if (m_screen)
        m_screen->OnRefreshGBScreen();
}

void Video::ClearScreen()
{
    if (m_screen)
        m_screen->OnClear();
}

void Video::UpdateBG(int y)
{
	
}

inline void Video::GetColor(VideoPixel *p)
{
	
}

void Video::UpdateOAM(int y)
{
	
}

void Video::GetColorPalette(BYTE palette[4][3], int address)
{
	
}
