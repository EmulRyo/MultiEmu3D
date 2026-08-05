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

#include <cmath>
#include <string>
#include <filesystem>
#include "Renderer3D.h"
#include "rlgl.h"

#define DEG2RAD 0.0174532925f

Renderer3D::Renderer3D()
    : m_is3DMode(false), m_modelLoaded(false)
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

    DrawOverlay(dst);
}

void Renderer3D::DrawAxis(const Vector3& origin, float length) {
    float radius = 0.02f;
    DrawCylinderEx(origin, { origin.x + length, origin.y, origin.z }, radius, radius, 8, RED);
    DrawCylinderEx(origin, { origin.x, origin.y + length, origin.z }, radius, radius, 8, GREEN);
    DrawCylinderEx(origin, { origin.x, origin.y, origin.z + length }, radius, radius, 8, BLUE);
}

void Renderer3D::OnChangeView() {
    m_is3DMode = !m_is3DMode;
    m_currentCamera = m_is3DMode ? m_camera3D : m_camera2D;
}

