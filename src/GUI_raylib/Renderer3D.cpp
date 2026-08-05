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

#include <cmath>
#include <string>
#include <filesystem>
#include "Renderer3D.h"
#include "rlgl.h"

#define DEG2RAD 0.0174532925f

Renderer3D::Renderer3D()
    : m_texture({}), m_is3DMode(false), m_modelLoaded(false)
{
    float fov = 28.0f;
    float h = 42.75f;
    float radFov = fov * DEG2RAD;
    float dist = (h / 2.0f) / tanf(radFov / 2.0f);
    float minZ = -(dist + 16.0f);

    // 2D camera: translate(0, -32.9, minZ), no rotation
    // Equivalent Raylib Camera3D position/target
    float camY = -32.9f;
    m_camera2D.position   = { 0.0f, -camY, -minZ };
    m_camera2D.target     = { 0.0f, -camY, -minZ - 1000.0f };
    m_camera2D.up         = { 0.0f, 1.0f, 0.0f };
    m_camera2D.fovy       = fov;
    m_camera2D.projection = CAMERA_PERSPECTIVE;

    m_camera3D.projection = CAMERA_PERSPECTIVE;
    m_camera3D.fovy = 60.0f;
    m_camera3D.up = { 0.0f, 1.0f, 0.0f };
    m_camera3D.position = { 0.025f, 0.075f, 0.2f };
    m_camera3D.target = { 0.0f, 0.075f, 0.0f };

    Mesh cubeMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    m_cube = LoadModelFromMesh(cubeMesh);

    m_currentCamera = m_camera2D;

    // Create screen texture
    Image img = GenImageColor(m_bufferWidth, m_bufferHeight, BLACK);
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    m_texture = LoadTextureFromImage(img);
    UnloadImage(img);

	m_gb = LoadModel("Models/gb.glb");

	Material* screenMat = &m_gb.materials[m_gb.materialCount - 1];
	screenMat->maps[MATERIAL_MAP_ALBEDO].color = WHITE;
    screenMat->maps[MATERIAL_MAP_ALBEDO].texture = m_texture;
}

Renderer3D::~Renderer3D() {
    if (m_texture.id > 0)
        UnloadTexture(m_texture);
}

void Renderer3D::Draw(Rectangle dst) {
    UpdateScreenTexture();

    BeginScissorMode((int)dst.x, (int)dst.y, (int)dst.width, (int)dst.height);

    BeginMode3D(m_camera3D);

	DrawGrid(10, 1.0f);
	//DrawAxis({ 0, 0, 0 }, 1.0f);

	DrawModel(m_gb, { 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);

    EndMode3D();

    EndScissorMode();

    DrawOverlays(dst);
}

void Renderer3D::DrawAxis(const Vector3& origin, float length) {
    float radius = 0.02f;
    DrawCylinderEx(origin, { origin.x + length, origin.y, origin.z }, radius, radius, 8, RED);
    DrawCylinderEx(origin, { origin.x, origin.y + length, origin.z }, radius, radius, 8, GREEN);
    DrawCylinderEx(origin, { origin.x, origin.y, origin.z + length }, radius, radius, 8, BLUE);
}

void Renderer3D::UpdateScreenTexture() {
    u8* ptr = m_frontBuffer + (m_x + m_y * m_width) * 3;
    UpdateTexture(m_texture, ptr);
}

void Renderer3D::OnChangeView() {
    m_is3DMode = !m_is3DMode;
    m_currentCamera = m_is3DMode ? m_camera3D : m_camera2D;
}

void Renderer3D::OnSizeChanged(int x, int y, int width, int height) {
    RendererBase::OnSizeChanged(x, y, width, height);

    UpdateTextureSize();
}

void Renderer3D::UpdateTextureSize() {
    if (m_texture.id > 0)
        UnloadTexture(m_texture);
    Image image = GenImageColor(m_width, m_height, BLACK);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    m_texture = LoadTextureFromImage(image);
    UnloadImage(image);
}

void Renderer3D::DrawOverlays(Rectangle dst) {
    if ((m_rewindValue < 0) && (m_iconFrames < 0))
        return;

    Color black{ 0, 0, 0, 255 };
    Color white{ 255, 255, 255, 200 };
    Color blue{ 0, 162, 255, 230 };

    if (m_iconFrames >= 0) {
        Rectangle iconDst = { dst.x + 12, dst.y + 12, dst.width - 12, dst.height - 12 };
        switch (m_icon) {
            case Renderer::Play: {
                float w = 14, h = 20;
                DrawTriangle(
                    { iconDst.x + w, iconDst.y + h / 2.0f },
                    { iconDst.x, iconDst.y },
                    { iconDst.x, iconDst.y + h }, white);
                DrawTriangleLines(
                    { iconDst.x + w, iconDst.y + h / 2.0f },
                    { iconDst.x, iconDst.y },
                    { iconDst.x, iconDst.y + h }, black);
                break;
            }
            case Renderer::Pause: {
                DrawRectangleRec({ iconDst.x, iconDst.y, 8, 20 }, white);
                DrawRectangleRec({ iconDst.x + 12, iconDst.y, 8, 20 }, white);
                DrawRectangleLinesEx({ iconDst.x, iconDst.y, 8, 20 }, 1, black);
                DrawRectangleLinesEx({ iconDst.x + 12, iconDst.y, 8, 20 }, 1, black);
                break;
            }
            case Renderer::Stop: {
                DrawRectangleRec({ iconDst.x, iconDst.y, 20, 20 }, white);
                DrawRectangleLinesEx({ iconDst.x, iconDst.y, 20, 20 }, 1, black);
                break;
            }
            default: break;
        }
        m_iconFrames--;
    }

    if (m_rewindValue >= 0) {
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

        DrawRectangleLinesEx({ x0 + 1, y0 + 1, x1-x0-2, y1-y0-2 }, 1, black);
        DrawRectangleLinesEx({ x0, y0, x1-x0, y1-y0 }, 1, white);
        DrawRectangleRec({ x0 + 1, y0+1, x2-x0-2, y1-y0-2 }, blue);
        DrawRectangleRec({ x3, y3, x4-x3, y4-y3 }, white);
        DrawRectangleLinesEx({ x3, y3, x4-x3, y4-y3 }, 1, black);
    }
}
