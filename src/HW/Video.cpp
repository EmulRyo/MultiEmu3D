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
    m_numWrite = 0;
    m_vramAddress = true;
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

void Video::UpdateLine(u8 y)
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

void Video::GetColorPalette(u8 palette[4][3], int address)
{
	
}

void Video::SetAddress(u8 value) {
    m_numWrite++;
    
    if (m_numWrite == 2) {
        u8 place = value >> 6;
        
        switch (place) {
            case 0x00:
            case 0x01:
            {
                m_address = ((value & 0x3F) << 8) | m_partialAddress;
                //printf("VRAM address: 0x%X\n", address);
                m_vramAddress = true;
                break;
            }
            case 0x02:
            {
                u8 reg = value & 0x0F;
                //printf("VDP reg: %d = 0x%X\n", reg, m_firstValueAddress);
                m_regs[reg] = m_partialAddress;
                break;
            }
            case 0x03:
            {
                m_address = m_partialAddress & 0x1F;
                //printf("VDP palette: %d\n", address);
                m_vramAddress = false;
                break;
            }
                
            default:
                break;
        }
        
        m_numWrite = 0;
    }
    
    m_partialAddress = value;
}


void Video::SetData(u8 value) {
    m_numWrite = 0;
    
    if (m_vramAddress) {
        if (m_address < 0x4000) {
            m_memory[m_address] = value;
            if (m_address < 0x3FFF)
                m_address++;
        }
    }
    else {
        if (m_address < 32) {
            m_palettes[m_address] = value;
            if (m_address < 31)
                m_address++;
        }
    }
}

u8 Video::GetAddress() {
    return m_partialAddress;
}

u8 Video::GetData() {
    if (m_vramAddress)
        return m_memory[m_address];
    else
        return m_palettes[m_address];
}
