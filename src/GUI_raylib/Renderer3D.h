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

#ifndef __RENDERER3D_H__
#define __RENDERER3D_H__

#include "RendererBase.h"
#include "raylib.h"

class Renderer3D : public RendererBase {

public:
    Renderer3D();
    ~Renderer3D();

    void Draw(Rectangle dst) override;
    void OnSizeChanged(int x, int y, int width, int height) override;
    void OnChangeView() override;

private:
    Model m_cube;
    Model m_gb;
    Texture2D m_texture;
    Camera3D m_camera2D;
    Camera3D m_camera3D;
    Camera3D m_currentCamera;
    bool m_is3DMode;
    bool m_modelLoaded;

    void UpdateScreenTexture();
    void UpdateTextureSize();
    void DrawOverlays(Rectangle dst);
	void DrawAxis(const Vector3& origin, float length);
};

#endif
