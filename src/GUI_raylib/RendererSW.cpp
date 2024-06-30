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

#include "RendererSW.h"
#include <stdio.h>

RendererSW::RendererSW()
{
    m_texture.id = 0;
    UpdateTextureSize();
}

RendererSW::~RendererSW() {
    
}

void RendererSW::OnSizeChanged(int x, int y, int width, int height) {
    RendererBase::OnSizeChanged(x, y, width, height);

    UpdateTextureSize();
}

void RendererSW::Draw(Rectangle dst) {
    float deviceAspectRatio = (float)m_width / m_height;
    float windowAspectRatio = dst.width / dst.height;
    Rectangle d{ 0 };
    if (windowAspectRatio > deviceAspectRatio) {
        d.width = dst.height * deviceAspectRatio;
        d.height = dst.height;
    }
    else {
        d.width = dst.width;
        d.height = dst.width / deviceAspectRatio;
    }
    d.x = dst.x + (dst.width-d.width) / 2.0f;
    d.y = dst.y + (dst.height-d.height) / 2.0f;

    u8* ptr = m_frontBuffer + (m_x + m_y * m_width) * 3;
    UpdateTexture(m_texture, ptr);
    DrawTexturePro(
        m_texture,
        Rectangle{ 0, 0, (float)m_width, (float)m_height },
        d,
        Vector2{ 0, 0 },
        0,
        WHITE);

    DrawOverlay(dst);
}

void RendererSW::UpdateTextureSize() {
    if (m_texture.id > 0)
        UnloadTexture(m_texture);
    Image image = GenImageColor(m_width, m_height, BLACK);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    m_texture = LoadTextureFromImage(image);
    UnloadImage(image);
}

void RendererSW::DrawOverlay(Rectangle dst) {
    if ((m_rewindValue < 0) && (m_iconFrames < 0))
        return;

    if (m_iconFrames >= 0)
        DrawIcon(dst);

    if (m_rewindValue >= 0)
        DrawRewind(dst);
}

void RendererSW::DrawIcon(Rectangle dst) {
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

void RendererSW::DrawIconRewindL(Rectangle dst) {
    const int width = 14;
    const int height = 20;
    const int offset = 14;
    float x0 = dst.x + 0.5f;
    float y0 = dst.y + height / 2 + 0.5f;
    float x1 = dst.x + width + 0.5f;
    float y1 = dst.y + height + 0.5f;
    float x2 = x1;
    float y2 = dst.y + 0.5f;

    DrawTriangle({ x0, y0 }, { x1, y1 }, { x2, y2 }, m_white);
    DrawTriangle({ x0 + offset, y0 }, { x1 + offset, y1 }, { x2 + offset, y2 }, m_white);

    DrawTriangleLines({ x0, y0 }, { x1, y1 }, { x2, y2 }, m_black);
    DrawTriangleLines({ x0 + offset, y0 }, { x1 + offset, y1 }, { x2 + offset, y2 }, m_black);
}

void RendererSW::DrawIconRewindR(Rectangle dst) {
    const int width = 14;
    const int height = 20;
    const int offset = 14;
    float x0 = dst.x + width + 0.5f;
    float y0 = dst.y + height / 2 + 0.5f;
    float x1 = dst.x + 0.5f;
    float y1 = dst.y + 0.5f;
    float x2 = x1;
    float y2 = dst.y + height + 0.5f;

    DrawTriangle({ x0, y0 }, { x1, y1 }, { x2, y2 }, m_white);
    DrawTriangle({ x0 + offset, y0 }, { x1 + offset, y1 }, { x2 + offset, y2 }, m_white);

    DrawTriangleLines({ x0, y0 }, { x1, y1 }, { x2, y2 }, m_black);
    DrawTriangleLines({ x0 + offset, y0 }, { x1 + offset, y1 }, { x2 + offset, y2 }, m_black);
}

void RendererSW::DrawIconArrow(Rectangle dst) {
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
    DrawTriangleFan(pointsFan, 6, m_white);
    Vector2 pointsStrip[] = {
        { x0, y0 }, { x1, y1 }, { x0, y2 }, { x2, y2 }, { x3, y1 },{ x2, y0 }, { x0, y0 }
    };
    DrawLineStrip(pointsStrip, 7, m_black);
}

void RendererSW::DrawIconPlay(Rectangle dst) {
    const float width = 14;
    const float height = 20;
    float x0 = dst.x + width;
    float y0 = dst.y + height / 2.0f;
    float x1 = dst.x;
    float y1 = dst.y;
    float x2 = x1;
    float y2 = dst.y + height;

    DrawTriangle({ x0, y0 }, { x1, y1 }, { x2, y2 }, m_white);
    DrawTriangleLines({ x0, y0 }, { x1, y1 }, { x2, y2 }, m_black);
}

void RendererSW::DrawIconSpeed(Rectangle dst) {
    DrawIconArrow(dst);
    DrawIconArrow({ dst.x + 12, dst.y, dst.width, dst.height });
    DrawIconArrow({ dst.x + 24, dst.y, dst.width, dst.height });
}

void RendererSW::DrawIconPause(Rectangle dst) {
    const int width = 8;
    const int height = 20;
    const int offset = 12;
    float x0 = dst.x;
    float y0 = dst.y;

    DrawRectangleRec({ x0, y0, width, height }, m_white);
    DrawRectangleRec({ x0+offset, y0, width, height }, m_white);

    DrawRectangleLinesEx({ x0, y0, width, height }, 1, m_black);
    DrawRectangleLinesEx({ x0 + offset, y0, width, height }, 1, m_black);
}

void RendererSW::DrawIconStop(Rectangle dst) {
    const int width = 20;
    const int height = 20;
    float x0 = dst.x;
    float y0 = dst.y;

    DrawRectangleRec({ x0, y0, width, height }, m_white);
    DrawRectangleLinesEx({ x0, y0, width, height }, 1, m_black);
}

void RendererSW::DrawRewind(Rectangle dst) {
    int gap = 12;
    int barHeight = 10;
    int cursorWidth = 6;
    int cursorHeightAdd = 3;

    float x0 = gap;
    float x1 = dst.width - gap;
    float y0 = dst.height - gap - barHeight;
    float y1 = dst.height - gap;
    float x2 = (int)(x0 + (x1 - x0) * m_rewindValue);
    float x3 = (int)(x2 - cursorWidth / 2.0f);
    float x4 = (int)(x2 + cursorWidth / 2.0f);
    float y3 = (int)(y0 - cursorHeightAdd);
    float y4 = (int)(y1 + cursorHeightAdd);

    DrawRectangleLinesEx({ x0 + 1, y0 + 1, x1-x0-2, y1-y0-2 }, 1, m_black);
    DrawRectangleLinesEx({ x0, y0, x1-x0, y1-y0 }, 1, m_white);
    
    DrawRectangleRec({ x0 + 1, y0+1, x2-x0-2, y1-y0-2 }, m_blue);
    DrawRectangleRec({ x3, y3, x4-x3, y4-y3 }, m_white);

    DrawRectangleLinesEx({ x3, y3, x4-x3, y4-y3 }, 1, m_black);
}
