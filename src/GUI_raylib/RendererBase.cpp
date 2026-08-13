/*
 This file is part of MARS.
 
 MARS is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MARS is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MARS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <math.h>
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
    m_bufferWidth = 256;
    m_bufferHeight = 256;
    m_x = m_y = 0;
    m_width = m_bufferWidth;
    m_height = m_bufferHeight;
    m_rewindValue = -1.0f;
    m_iconFrames = -1;
    m_gbPalette = 0;
	CreateBuffers();

    m_texture.id = 0;
    UpdateTextureSize();
}

RendererBase::~RendererBase()
{
    if (m_imgBuf1)
        delete[] m_imgBuf1;
    
    if (m_imgBuf2)
        delete[] m_imgBuf2;
}

void RendererBase::CreateBuffers() {
	m_imgBuf1 = new u8[m_bufferWidth * m_bufferHeight * 3];
    m_imgBuf2 = new u8[m_bufferWidth * m_bufferHeight * 3];
    m_backBuffer = m_imgBuf1;
    m_frontBuffer = m_imgBuf2;
	OnClear();
}

void RendererBase::OnClear()
{
    u8 r = gbPalettes[m_gbPalette][3][0];
    u8 g = gbPalettes[m_gbPalette][3][1];
    u8 b = gbPalettes[m_gbPalette][3][2];
    
    size_t sizeLine = m_width * 3;
    for (size_t y = 0; y < m_height; y++) {
        size_t offsetY = y * sizeLine;
        for (size_t x = 0; x < m_width; x++) {
            size_t offsetX = x * 3;
            size_t offsetBuf = offsetY + offsetX;
            m_backBuffer[offsetBuf + 0] = m_frontBuffer[offsetBuf + 0] = r;
            m_backBuffer[offsetBuf + 1] = m_frontBuffer[offsetBuf + 1] = g;
            m_backBuffer[offsetBuf + 2] = m_frontBuffer[offsetBuf + 2] = b;
        }
    }

	PageFlip();
}

u8* RendererBase::GetBufferPtr() const {
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

    UpdateTextureSize();
}

int RendererBase::GetMinimunWidth() const{
    return m_width;
}

int RendererBase::GetMinimunHeight() const{
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

void RendererBase::UpdateScreenTexture() {
    u8* ptr = m_frontBuffer + (m_x + m_y * m_width) * 3;
    UpdateTexture(m_texture, ptr);
}

void RendererBase::UpdateTextureSize() {
    if (m_texture.id > 0)
        UnloadTexture(m_texture);
    Image image = GenImageColor(m_width, m_height, BLACK);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    m_texture = LoadTextureFromImage(image);
    UnloadImage(image);
}

void RendererBase::DrawOverlay(Rectangle dst) {
    if ((m_rewindValue < 0) && (m_iconFrames < 0))
        return;

    if (m_iconFrames >= 0)
        DrawIcon(dst);

    if (m_rewindValue >= 0)
        DrawRewind(dst);
}

void RendererBase::DrawIcon(Rectangle dst) {
    dst.x += 12;
    dst.y += 12;
    dst.width -= 12;
    dst.height -= 12;

    switch (m_icon) {
    case Renderer::RewindL:     DrawIconRewindL(dst); break;
    case Renderer::RewindR:     DrawIconRewindR(dst); break;
    case Renderer::Play:        DrawIconPlay(dst);    break;
    case Renderer::Pause:       DrawIconPause(dst);   break;
    case Renderer::Stop:        DrawIconStop(dst);    break;
    case Renderer::MaxSpeed:    DrawIconSpeed(dst);   break;

    default: break;
    }

    m_iconFrames--;
}

void RendererBase::DrawIconRewindL(Rectangle dst) {
    const int width = 14;
    const int height = 20;
    const int offset = 14;
    float x0 = dst.x + 0.5f;
    float y0 = dst.y + height / 2 + 0.5f;
    float x1 = dst.x + width + 0.5f;
    float y1 = dst.y + height + 0.5f;
    float x2 = x1;
    float y2 = dst.y + 0.5f;

    DrawTriangle({ x0, y0 }, { x1, y1 }, { x2, y2 }, WHITE);
    DrawTriangle({ x0 + offset, y0 }, { x1 + offset, y1 }, { x2 + offset, y2 }, WHITE);

    DrawTriangleLines({ x0, y0 }, { x1, y1 }, { x2, y2 }, BLACK);
    DrawTriangleLines({ x0 + offset, y0 }, { x1 + offset, y1 }, { x2 + offset, y2 }, BLACK);
}

void RendererBase::DrawIconRewindR(Rectangle dst) {
    const int width = 14;
    const int height = 20;
    const int offset = 14;
    float x0 = dst.x + width + 0.5f;
    float y0 = dst.y + height / 2 + 0.5f;
    float x1 = dst.x + 0.5f;
    float y1 = dst.y + 0.5f;
    float x2 = x1;
    float y2 = dst.y + height + 0.5f;

    DrawTriangle({ x0, y0 }, { x1, y1 }, { x2, y2 }, WHITE);
    DrawTriangle({ x0 + offset, y0 }, { x1 + offset, y1 }, { x2 + offset, y2 }, WHITE);

    DrawTriangleLines({ x0, y0 }, { x1, y1 }, { x2, y2 }, BLACK);
    DrawTriangleLines({ x0 + offset, y0 }, { x1 + offset, y1 }, { x2 + offset, y2 }, BLACK);
}

void RendererBase::DrawIconArrow(Rectangle dst) {
    const float width = 12;
    const float height = 20;
    float x0 = dst.x;
    float y0 = dst.y;
    float x1 = dst.x + width / 3.0f;
    float y1 = dst.y + height / 2.0f;
    float x2 = dst.x + 2 * width / 3.0f;
    float y2 = dst.y + height;
    float x3 = dst.x + width;

    Vector2 pointsFan[] = {
        { x1, y1 }, { x0, y2 }, { x2, y2 }, { x3, y1 },{ x2, y0 }, { x0, y0 }
    };
    DrawTriangleFan(pointsFan, 6, WHITE);
    Vector2 pointsStrip[] = {
        { x0, y0 }, { x1, y1 }, { x0, y2 }, { x2, y2 }, { x3, y1 },{ x2, y0 }, { x0, y0 }
    };
    DrawLineStrip(pointsStrip, 7, BLACK);
}

void RendererBase::DrawIconPlay(Rectangle dst) {
    const float width = 14;
    const float height = 20;
    float x0 = dst.x + width;
    float y0 = dst.y + height / 2.0f;
    float x1 = dst.x;
    float y1 = dst.y;
    float x2 = x1;
    float y2 = dst.y + height;

    DrawTriangle({ x0, y0 }, { x1, y1 }, { x2, y2 }, WHITE);
    DrawTriangleLines({ x0, y0 }, { x1, y1 }, { x2, y2 }, BLACK);
}

void RendererBase::DrawIconSpeed(Rectangle dst) {
    DrawIconArrow(dst);
    DrawIconArrow({ dst.x + 12, dst.y, dst.width, dst.height });
    DrawIconArrow({ dst.x + 24, dst.y, dst.width, dst.height });
}

void RendererBase::DrawIconPause(Rectangle dst) {
    const int width = 8;
    const int height = 20;
    const int offset = 12;
    float x0 = dst.x;
    float y0 = dst.y;

    DrawRectangleRec({ x0, y0, width, height }, WHITE);
    DrawRectangleRec({ x0 + offset, y0, width, height }, WHITE);

    DrawRectangleLinesEx({ x0, y0, width, height }, 1, BLACK);
    DrawRectangleLinesEx({ x0 + offset, y0, width, height }, 1, BLACK);
}

void RendererBase::DrawIconStop(Rectangle dst) {
    const int width = 20;
    const int height = 20;
    float x0 = dst.x;
    float y0 = dst.y;

    DrawRectangleRec({ x0, y0, width, height }, WHITE);
    DrawRectangleLinesEx({ x0, y0, width, height }, 1, BLACK);
}

void RendererBase::DrawRewind(Rectangle dst) {
    float gap = 12.0f;
    float barHeight = 10.0f;
    float cursorWidth = 6.0f;
    float cursorHeightAdd = 3.0f;

    float x0 = gap;
    float x1 = dst.width - gap;
    float y0 = dst.height - gap - barHeight;
    float y1 = dst.height - gap;
    float x2 = floorf(x0 + (x1 - x0) * m_rewindValue);
    float x3 = floorf(x2 - cursorWidth / 2.0f);
    float x4 = floorf(x2 + cursorWidth / 2.0f);
    float y3 = floorf(y0 - cursorHeightAdd);
    float y4 = floorf(y1 + cursorHeightAdd);
    DrawRectangleLinesEx({ x0 + 1, y0 + 1, x1 - x0 - 2, y1 - y0 - 2 }, 1, BLACK);
    DrawRectangleLinesEx({ x0, y0, x1 - x0, y1 - y0 }, 1, WHITE);

    DrawRectangleRec({ x0 + 1, y0 + 1, x2 - x0 - 2, y1 - y0 - 2 }, BLUE);
    DrawRectangleRec({ x3, y3, x4 - x3, y4 - y3 }, WHITE);

    DrawRectangleLinesEx({ x3, y3, x4 - x3, y4 - y3 }, 1, BLACK);
}