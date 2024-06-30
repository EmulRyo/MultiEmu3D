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

#ifndef __RENDERERSW_H__
#define __RENDERERSW_H__

#include "RendererBase.h"
#include "raylib.h"

/*******************************************************************************
 * RendererSW Class
 *******************************************************************************/

class RendererSW : public RendererBase {

public:
	RendererSW();
    ~RendererSW();

	void Draw(Rectangle dst);
    void OnSizeChanged(int x, int y, int width, int height) override;
    void OnChangeView() {};
    
private:
    Texture2D m_texture;
    Color m_black {   0,   0,   0, 255 };
    Color m_white { 255, 255, 255, 200 };
    Color m_blue  {   0, 162, 255, 230 };
    
    void UpdateTextureSize();
    void DrawOverlay(Rectangle dst);
    void DrawIcon(Rectangle dst);
    void DrawRewind(Rectangle dst);

    void DrawIconRewindL(Rectangle dst);
    void DrawIconRewindR(Rectangle dst);
    void DrawIconPlay(Rectangle dst);
    void DrawIconPause(Rectangle dst);
    void DrawIconStop(Rectangle dst);
    void DrawIconSpeed(Rectangle dst);
    void DrawIconArrow(Rectangle dst);
};

#endif
