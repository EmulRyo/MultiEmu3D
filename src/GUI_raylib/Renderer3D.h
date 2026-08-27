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

#ifndef __RENDERER3D_H__
#define __RENDERER3D_H__

#include "RendererBase.h"
#include "raylib.h"

class Renderer3D : public RendererBase {

public:
    Renderer3D();
    ~Renderer3D();

    void Update(float deltaTime) override;
    void Draw(const Rectangle& dst) override;
    void OnChangeView() override;

private:
    struct CameraAnim{
        Vector3    position;
        Quaternion rotation;
        float      distance;
        float      fovy;
    };
    Rectangle m_drawArea;
    Model m_gb;
    Model m_ground;
    Shader m_pbrShader;
    Shader m_screenShader;
    Shader m_depthShader;
	Shader m_grayscaleTextureShader;
    RenderTexture2D m_shadowMap;
    Camera3D m_camera2D;
    Camera3D m_camera3D;
    Camera3D m_currentCamera;
    bool m_is3DMode;
    bool m_modelLoaded;
    bool m_showShadowMap;
    
    bool m_transitioning;
	CameraAnim m_startAnim;
	CameraAnim m_targetAnim;
    float m_transitionTime;
    int m_screenMaterial;
    int m_lightVPLoc;
    int m_shadowMapLoc;
    int m_shadowTexelSizeLoc;

	void DrawAxis(const Vector3& origin, float length);
	void DrawScene();
	void DrawShadowPass();
	void SetSceneShader(Shader shader);
	void RestoreSceneShaders();
	void UpdateCamera2D(const Rectangle& dst);
	bool AreaChanged(const Rectangle& dst) const;
	void OnAreaChanged(const Rectangle& dst);
	void CameraStartAnimation();
    void CameraAnimate(float deltaTime);
    void CameraControl(float deltaTime);
};

#endif
