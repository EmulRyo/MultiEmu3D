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

#include "../NES/Def.h"
#include "RendererSW.h"


RendererSW::RendererSW()
{
	Image image = GenImageColor(NES_SCREEN_W, NES_SCREEN_H, BLACK);
	ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
	m_texture = LoadTextureFromImage(image);
	UnloadImage(image);
}

RendererSW::~RendererSW() {
    
}

void RendererSW::Draw() {
    
	UpdateTexture(m_texture, m_frontBuffer);	
	DrawTexture(m_texture, 0, 0, WHITE);
    DrawTexturePro(
        m_texture,
        Rectangle{ 0, 0, NES_SCREEN_W, NES_SCREEN_H },
        Rectangle{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() },
        Vector2{ 0, 0 },
        0,
        WHITE);
}

