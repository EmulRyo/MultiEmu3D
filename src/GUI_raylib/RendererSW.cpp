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
}

void RendererSW::UpdateTextureSize() {
    if (m_texture.id > 0)
        UnloadTexture(m_texture);
    Image image = GenImageColor(m_width, m_height, BLACK);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    m_texture = LoadTextureFromImage(image);
    UnloadImage(image);
}