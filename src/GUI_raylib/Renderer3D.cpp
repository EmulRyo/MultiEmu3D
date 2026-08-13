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
#include "rcamera.h"
#include "raymath.h"
#include "rlgl.h"

Renderer3D::Renderer3D():
	m_camera2D({ 0 }),
    m_drawArea({0, 0, 0, 0}),
    m_is3DMode(false),
    m_modelLoaded(false),
    m_transitioning(false),
    m_transitionTime(0.0f),
	m_startAnim({ {0,0,0}, {0,0,0,1}, 1.0f, 60.0f }),
	m_targetAnim({ {0,0,0}, {0,0,0,1}, 1.0f, 60.0f })
{

	UpdateCamera2D({ 0, 0, (float)m_width, (float)m_height });

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

static Quaternion GetCameraRotation(Camera3D& camera) {
    return QuaternionFromMatrix(MatrixInvert(GetCameraViewMatrix(&camera)));
}

void Renderer3D::Update(float deltaTime) {
    if (m_transitioning)
		CameraAnimate(deltaTime);
    else
		CameraControl(deltaTime);
}

void Renderer3D::CameraAnimate(float deltaTime) {
    const float transitionDuration = 2.0f;
    m_transitionTime += deltaTime;
    float amount = fminf(m_transitionTime / transitionDuration, 1.0f);
    float smoothAmount = amount * amount * (3.0f - 2.0f * amount);

    m_currentCamera.position = Vector3Lerp(m_startAnim.position, m_targetAnim.position, smoothAmount);

    float distance = Lerp(m_startAnim.distance, m_targetAnim.distance, smoothAmount);
    Quaternion rotation = QuaternionSlerp(m_startAnim.rotation, m_targetAnim.rotation, smoothAmount);
    Vector3 forward = Vector3RotateByQuaternion({ 0.0f, 0.0f, -1.0f }, rotation);
    Vector3 up = Vector3RotateByQuaternion({ 0.0f, 1.0f, 0.0f }, rotation);

    m_currentCamera.target = Vector3Add(m_currentCamera.position, Vector3Scale(forward, distance));
    m_currentCamera.up = up;
    m_currentCamera.fovy = Lerp(m_startAnim.fovy, m_targetAnim.fovy, smoothAmount);

    if (amount >= 1.0f) {
        m_transitioning = false;
    }
}

void Renderer3D::CameraControl(float deltaTime) {
    Vector2 mouseDelta = GetMouseDelta();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        CameraYaw(&m_currentCamera, -mouseDelta.x * DEG2RAD * 0.1f, true);
        CameraPitch(&m_currentCamera, -mouseDelta.y * DEG2RAD * 0.1f,
            true, true, false);
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        CameraMoveRight(&m_currentCamera, mouseDelta.x * 0.0001f, true);
        CameraMoveUp(&m_currentCamera, -mouseDelta.y * 0.0001f);
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        CameraMoveToTarget(&m_currentCamera, -wheel);
    }

    float distance = Vector3Distance(m_currentCamera.position, m_currentCamera.target);
    if (distance < 0.05f)
        CameraMoveToTarget(&m_currentCamera, 0.05f - distance);
    else if (distance > 20.0f)
        CameraMoveToTarget(&m_currentCamera, 20.0f - distance);
}

void Renderer3D::Draw(const Rectangle& dst) {
    UpdateScreenTexture();

    if (AreaChanged(dst)) {
        m_drawArea = dst;
		OnAreaChanged(dst);
	}

    BeginScissorMode((int)dst.x, (int)dst.y, (int)dst.width, (int)dst.height);

    BeginMode3D(m_currentCamera);

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

    CameraStartAnimation();
}

void Renderer3D::CameraStartAnimation() {
    Camera3D src, dst;
    if (m_is3DMode == false) {
        src = m_currentCamera;
        dst = m_camera2D;
    }
    else {
        src = m_currentCamera;
        dst = m_camera3D;
    }

    m_startAnim.position = src.position;
    m_startAnim.rotation = GetCameraRotation(src);
    m_startAnim.distance = Vector3Distance(src.position, src.target);
    m_startAnim.fovy = src.fovy;

    m_targetAnim.position = dst.position;
    m_targetAnim.rotation = GetCameraRotation(dst);
    m_targetAnim.distance = Vector3Distance(dst.position, dst.target);
    m_targetAnim.fovy = dst.fovy;

    m_transitionTime = 0.0f;
    m_transitioning = true;
}

bool Renderer3D::AreaChanged(const Rectangle& dst) const {
    return (m_drawArea.x != dst.x) || (m_drawArea.y != dst.y) || (m_drawArea.width != dst.width) || (m_drawArea.height != dst.height);
}

void Renderer3D::OnAreaChanged(const Rectangle& dst) {
    if (dst.height > 0.0f && GetRenderHeight() > 0)
        UpdateCamera2D(dst);

    CameraStartAnimation();
}

void Renderer3D::UpdateCamera2D(const Rectangle& dst) {
    // Raylib builds the projection using the whole framebuffer. Adjust
    // the camera copy to the actual scissored viewport instead of
    // changing the camera state or assuming fixed UI dimensions.
    float fov = 28.0f;
    // The glTF export uses Y-up coordinates: Blender Z becomes application Y
    // and Blender -Y becomes application Z.
    const float screenZ = 0.016052f;
    const float screenBottomY = 0.084879f;
    const float screenTopY = 0.12732f;
    const float screenHeight = screenTopY - screenBottomY;
    const float screenCenterY = (screenTopY + screenBottomY) / 2.0f;
    const float radFov = fov * DEG2RAD;
    const float dist = screenHeight / (2.0f * tanf(radFov / 2.0f));

    m_camera2D.position = { 0.0f, screenCenterY, screenZ + dist };
    m_camera2D.target = { 0.0f, screenCenterY, screenZ };
    m_camera2D.up = { 0.0f, 1.0f, 0.0f };
    m_camera2D.fovy = fov;
    m_camera2D.projection = CAMERA_PERSPECTIVE;

    const float framebufferHeight = (float)GetRenderHeight();
    const float viewportCenter = dst.y + dst.height * 0.5f;
    const float framebufferCenter = framebufferHeight * 0.5f;
    const float fitScale = framebufferHeight / dst.height;

    Vector3 view = Vector3Subtract(m_camera2D.position, m_camera2D.target);
    m_camera2D.position = Vector3Add(m_camera2D.target, Vector3Scale(view, fitScale));

    const float visibleHeight = 2.0f * Vector3Distance(m_camera2D.position, m_camera2D.target) * tanf(m_camera2D.fovy * DEG2RAD * 0.5f);
    const float centerOffset = (viewportCenter - framebufferCenter) / framebufferHeight * visibleHeight;
    m_camera2D.position.y += centerOffset;
    m_camera2D.target.y += centerOffset;
}
