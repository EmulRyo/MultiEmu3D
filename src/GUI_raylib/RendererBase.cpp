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

#include <string.h>
#include "RendererBase.h"
#include "../NES/Def.h"

static u8 gbPalettes[][4][3] = {
    {
        { 16,  57,  16},
        { 49,  99,  49},
        {140, 173,  16},
        {156, 189,  16}
    },
    {
        {  0,   0,   0},
        { 85,  85,  85},
        {170, 170, 170},
        {255, 255, 255}
    }
};

RendererBase::RendererBase()
{
    m_imgBuf1 = nullptr;
    m_imgBuf2 = nullptr;
    m_frontBuffer = nullptr;
    m_backBuffer = nullptr;
    m_x = m_y = 0;
    m_width = 256;
    m_height = 256;
    m_rewindValue = -1.0f;
    m_iconFrames = -1;
    m_gbPalette = 0;
	CreateScreen();
}

RendererBase::~RendererBase()
{
    if (m_imgBuf1)
        delete[] m_imgBuf1;
    
    if (m_imgBuf2)
        delete[] m_imgBuf2;
}

void RendererBase::CreateScreen() {
	m_imgBuf1 = new u8[m_width * m_height * 3];
    m_imgBuf2 = new u8[m_width * m_height * 3];
    m_backBuffer = m_imgBuf1;
    m_frontBuffer = m_imgBuf2;
	OnClear();
}

void RendererBase::OnClear()
{
	int size = m_width * m_height * 3;
    memset(m_backBuffer, 0, size);
    memset(m_frontBuffer, 0, size);
	PageFlip();
}

u8* RendererBase::GetBufferPtr() {
    return m_frontBuffer;
}

void RendererBase::PageFlip()
{
    u8 *aux = m_frontBuffer;
    m_frontBuffer = m_backBuffer;
    m_backBuffer = aux;
}

void RendererBase::OnRefreshEmulatedScreen()
{
	PageFlip();
}

void RendererBase::OnDrawPixel(int idColor, int x, int y) {
    u8 r = gbPalettes[m_gbPalette][idColor][0];
    u8 g = gbPalettes[m_gbPalette][idColor][1];
    u8 b = gbPalettes[m_gbPalette][idColor][2];
    
    OnDrawPixel(r, g, b, x, y);
}

void RendererBase::OnDrawPixel(u8 r, u8 g, u8 b, int x, int y)
{
    int sizeLine = m_width * 3;
	int offsetX = x * 3;
	int offsetY = y * sizeLine;
	int offsetBuf = offsetY + offsetX;
	
	m_backBuffer[offsetBuf + 0] = r;
	m_backBuffer[offsetBuf + 1] = g;
	m_backBuffer[offsetBuf + 2] = b;
}

void RendererBase::OnSizeChanged(int x, int y, int width, int height) {
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
}

int RendererBase::GetMinimunWidth() {
    return m_width;
}

int RendererBase::GetMinimunHeight() {
    return m_height;
}

void RendererBase::SetRewindValue(float value) {
    m_rewindValue = value;
}

void RendererBase::SetIcon(Renderer::Icon icon, int frames) {
    m_icon = icon;
    m_iconFrames = frames;
}

void RendererBase::SetGBPalette(bool original)
{
    if (original)
        m_gbPalette = 0;
    else
        m_gbPalette = 1;
}
