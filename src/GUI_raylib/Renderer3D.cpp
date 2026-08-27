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

namespace {
constexpr int SHADOWMAP_RESOLUTION = 1024;

RenderTexture2D LoadShadowmapRenderTexture(int width, int height) {
    RenderTexture2D target = { 0 };
    target.id = rlLoadFramebuffer();
    target.texture.width = width;
    target.texture.height = height;

    if (target.id > 0) {
        rlEnableFramebuffer(target.id);
        target.depth.id = rlLoadTextureDepth(width, height, false);
        target.depth.width = width;
        target.depth.height = height;
        target.depth.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
        target.depth.mipmaps = 1;
        rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);
        rlDisableFramebuffer();
    }

    return target;
}

void UnloadShadowmapRenderTexture(RenderTexture2D target) {
    if (target.id > 0)
        rlUnloadFramebuffer(target.id);
}

}

Renderer3D::Renderer3D():
	m_camera2D({ 0 }),
    m_drawArea({0, 0, 0, 0}),
    m_is3DMode(false),
    m_modelLoaded(false),
    m_showShadowMap(false),
    m_transitioning(false),
    m_transitionTime(0.0f),
	m_screenMaterial(-1),
	m_lightVPLoc(-1),
	m_shadowMapLoc(-1),
	m_shadowTexelSizeLoc(-1),
	m_startAnim({ {0,0,0}, {0,0,0,1}, 1.0f, 60.0f }),
	m_targetAnim({ {0,0,0}, {0,0,0,1}, 1.0f, 60.0f })
{

	UpdateCamera2D({ 0, 0, (float)m_width, (float)m_height });

    m_camera3D.projection = CAMERA_PERSPECTIVE;
    m_camera3D.fovy = 60.0f;
    m_camera3D.up = { 0.0f, 1.0f, 0.0f };
    m_camera3D.position = { 0.025f, 0.075f, 0.2f };
    m_camera3D.target = { 0.0f, 0.075f, 0.0f };

    m_currentCamera = m_camera2D;

    // Create screen texture
    Image img = GenImageColor(m_bufferWidth, m_bufferHeight, BLACK);
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    m_texture = LoadTextureFromImage(img);
    UnloadImage(img);

	m_gb = LoadModel("Models/gb.glb");
    //m_gb = LoadModel("Models/old_car_new.glb");

    m_pbrShader = LoadShader("Shaders/pbr_shadow.vs", "Shaders/pbr_shadow.fs");
    m_screenShader = LoadShader("", "Shaders/screen.fs");
    m_depthShader = LoadShader("", "Shaders/depth.fs");
	m_grayscaleTextureShader = LoadShader("", "Shaders/grayscale_texture.fs");

    m_pbrShader.locs[SHADER_LOC_MAP_ALBEDO] = GetShaderLocation(m_pbrShader, "texture0");
    m_pbrShader.locs[SHADER_LOC_COLOR_DIFFUSE] = GetShaderLocation(m_pbrShader, "colDiffuse");
    m_pbrShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(m_pbrShader, "viewPos");
    m_pbrShader.locs[SHADER_LOC_MATRIX_NORMAL] = GetShaderLocation(m_pbrShader, "matNormal");
    m_screenShader.locs[SHADER_LOC_MAP_ALBEDO] = GetShaderLocation(m_screenShader, "texture0");
    m_screenShader.locs[SHADER_LOC_COLOR_DIFFUSE] = GetShaderLocation(m_screenShader, "colDiffuse");

    m_lightVPLoc = GetShaderLocation(m_pbrShader, "lightVP");
    m_shadowMapLoc = GetShaderLocation(m_pbrShader, "shadowMap");
    // Let raylib bind the shadow map with each PBR material instead of relying
    // on an external texture slot that DrawMesh() may change.
    m_pbrShader.locs[SHADER_LOC_MAP_HEIGHT] = m_shadowMapLoc;
    m_shadowTexelSizeLoc = GetShaderLocation(m_pbrShader, "shadowTexelSize");
    const float shadowTexelSize = 1.0f/SHADOWMAP_RESOLUTION;
    SetShaderValue(m_pbrShader, m_shadowTexelSizeLoc, &shadowTexelSize, SHADER_UNIFORM_FLOAT);

    const Vector3 lightDirection = Vector3Normalize({ 0.35f, -1.0f, -0.35f });
    const Vector3 lightColor = { 3.0f, 2.85f, 2.6f };
    const Vector3 ambientColor = { 0.08f, 0.09f, 0.12f };
    const float metallic = 0.05f;
    const float roughness = 0.62f;
    SetShaderValue(m_pbrShader, GetShaderLocation(m_pbrShader, "lightDir"), &lightDirection, SHADER_UNIFORM_VEC3);
    SetShaderValue(m_pbrShader, GetShaderLocation(m_pbrShader, "lightColor"), &lightColor, SHADER_UNIFORM_VEC3);
    SetShaderValue(m_pbrShader, GetShaderLocation(m_pbrShader, "ambientColor"), &ambientColor, SHADER_UNIFORM_VEC3);
    SetShaderValue(m_pbrShader, GetShaderLocation(m_pbrShader, "metallic"), &metallic, SHADER_UNIFORM_FLOAT);
    SetShaderValue(m_pbrShader, GetShaderLocation(m_pbrShader, "roughness"), &roughness, SHADER_UNIFORM_FLOAT);

    for (int i = 0; i < m_gb.materialCount; i++)
        m_gb.materials[i].shader = m_pbrShader;

    if (m_gb.materialCount > 0) {
        m_screenMaterial = m_gb.materialCount - 1;
        Material* screenMat = &m_gb.materials[m_screenMaterial];
        screenMat->shader = m_screenShader;
        screenMat->maps[MATERIAL_MAP_ALBEDO].color = WHITE;
        screenMat->maps[MATERIAL_MAP_ALBEDO].texture = m_texture;
    }

    m_ground = LoadModelFromMesh(GenMeshPlane(100.f, 100.f, 1, 1));
    m_ground.materials[0].shader = m_pbrShader;
    m_ground.materials[0].maps[MATERIAL_MAP_ALBEDO].color = { 42, 47, 56, 255 };
    m_shadowMap = LoadShadowmapRenderTexture(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);

    for (int i = 0; i < m_gb.materialCount; i++) {
        if (i != m_screenMaterial)
            m_gb.materials[i].maps[MATERIAL_MAP_HEIGHT].texture = m_shadowMap.depth;
    }
    m_ground.materials[0].maps[MATERIAL_MAP_HEIGHT].texture = m_shadowMap.depth;

    rlSetClipPlanes(0.01, 100.0);
}

Renderer3D::~Renderer3D() {
    // Models own their material shaders and textures, so detach shared resources first.
    Shader defaultShader = { 0 };
    defaultShader.id = rlGetShaderIdDefault();
    Texture2D defaultTexture = { 0 };
    defaultTexture.id = rlGetTextureIdDefault();
    for (int i = 0; i < m_gb.materialCount; i++) {
        m_gb.materials[i].shader = defaultShader;
        m_gb.materials[i].maps[MATERIAL_MAP_HEIGHT].texture = defaultTexture;
        if (i == m_screenMaterial)
            m_gb.materials[i].maps[MATERIAL_MAP_ALBEDO].texture = defaultTexture;
    }
    if (m_ground.materialCount > 0) {
        m_ground.materials[0].shader = defaultShader;
        m_ground.materials[0].maps[MATERIAL_MAP_HEIGHT].texture = defaultTexture;
    }

    UnloadModel(m_ground);
    UnloadModel(m_gb);
    UnloadShadowmapRenderTexture(m_shadowMap);
    UnloadShader(m_depthShader);
    UnloadShader(m_screenShader);
    UnloadShader(m_pbrShader);
    if (m_texture.id > 0) UnloadTexture(m_texture);
}

static Quaternion GetCameraRotation(Camera3D& camera) {
    return QuaternionFromMatrix(MatrixInvert(GetCameraViewMatrix(&camera)));
}

void Renderer3D::Update(float deltaTime) {
    if (IsKeyPressed(KEY_F3))
        m_showShadowMap = !m_showShadowMap;

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
    if (distance < 0.07f)
        CameraMoveToTarget(&m_currentCamera, 0.07f - distance);
    else if (distance > 10.0f)
        CameraMoveToTarget(&m_currentCamera, 10.0f - distance);
}

void Renderer3D::Draw(const Rectangle& dst) {
    UpdateScreenTexture();

    if (AreaChanged(dst)) {
        m_drawArea = dst;
		OnAreaChanged(dst);
	}

    DrawShadowPass();

    BeginScissorMode((int)dst.x, (int)dst.y, (int)dst.width, (int)dst.height);

    SetShaderValue(m_pbrShader, m_pbrShader.locs[SHADER_LOC_VECTOR_VIEW], &m_currentCamera.position, SHADER_UNIFORM_VEC3);
    BeginMode3D(m_currentCamera);

    DrawScene();

    EndMode3D();

    EndScissorMode();

    if (m_showShadowMap) {
        BeginShaderMode(m_grayscaleTextureShader);
        DrawTexturePro(m_shadowMap.depth,
            { 0.0f, 0.0f, (float)m_shadowMap.depth.width, (float)m_shadowMap.depth.height },
            { 12.0f, 92.0f, 192.0f, 192.0f }, { 0.0f, 0.0f }, 0.0f, WHITE);
		EndShaderMode();
        DrawText("Shadow depth (F3)", 12, 72, 14, RAYWHITE);
    }

    DrawOverlay(dst);
}

void Renderer3D::DrawScene() {
    DrawModel(m_ground, { 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);
    DrawModel(m_gb, { 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);
}

void Renderer3D::DrawShadowPass() {
    const Vector3 lightDirection = Vector3Normalize({ 0.35f, -1.0f, -0.35f });
    Camera3D lightCamera = { 0 };
    lightCamera.position = lightDirection * -1.5f;
    lightCamera.target = { 0.0f, 0.06f, 0.0f };
    lightCamera.up = { 0.0f, 1.0f, 0.0f };
    lightCamera.projection = CAMERA_ORTHOGRAPHIC;
    lightCamera.fovy = 0.2f;

    SetSceneShader(m_depthShader);
    const double previousNearPlane = rlGetCullDistanceNear();
    const double previousFarPlane = rlGetCullDistanceFar();
    // The default far plane is intended for the whole scene. Restricting it
    // here preserves enough depth precision for a model measured in centimetres.
    rlSetClipPlanes(1.0, 3.0);
    BeginTextureMode(m_shadowMap);
        ClearBackground(WHITE);
        BeginMode3D(lightCamera);
            Matrix lightView = rlGetMatrixModelview();
            Matrix lightProjection = rlGetMatrixProjection();
            DrawScene();
            SetShaderValueMatrix(m_pbrShader, m_lightVPLoc, MatrixMultiply(lightView, lightProjection));
        EndMode3D();
    EndTextureMode();
    rlSetClipPlanes(previousNearPlane, previousFarPlane);
    RestoreSceneShaders();
}

void Renderer3D::SetSceneShader(Shader shader) {
    for (int i = 0; i < m_gb.materialCount; i++)
        m_gb.materials[i].shader = shader;
    if (m_ground.materialCount > 0)
        m_ground.materials[0].shader = shader;
}

void Renderer3D::RestoreSceneShaders() {
    for (int i = 0; i < m_gb.materialCount; i++)
        m_gb.materials[i].shader = (i == m_screenMaterial)? m_screenShader : m_pbrShader;
    if (m_ground.materialCount > 0)
        m_ground.materials[0].shader = m_pbrShader;
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

    Vector3 view = m_camera2D.position - m_camera2D.target;
    m_camera2D.position = m_camera2D.target + (view * fitScale);

    const float visibleHeight = 2.0f * Vector3Distance(m_camera2D.position, m_camera2D.target) * tanf(m_camera2D.fovy * DEG2RAD * 0.5f);
    const float centerOffset = (viewportCenter - framebufferCenter) / framebufferHeight * visibleHeight;
    m_camera2D.position.y += centerOffset;
    m_camera2D.target.y += centerOffset;
}
