//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "haru/render/DeferredShaders.h"
#include "haru/render/Framebuffer.h"
#include "haru/render/Material.h"
#include "haru/render/Mesh.h"
#include "haru/render/MeshPositionColor.h"
#include "haru/render/Renderer.h"
#include "haru/render/ShadowMap.h"
#include "haru/render/ShadowMatrixCalculator.h"
#include "haru/render/Texture.h"
#include "haru/render/UniformBuffer.h"

class DeferredRenderer final : public Renderer {
    struct ShaderGlobals {
        glm::mat4 View{1.0f};
        glm::mat4 Projection{1.0f};
    };

    struct PointLightData {
        glm::vec3 Position{0.0f, 0.0f, 0.0f};
        float Linear = 0.0f;
        glm::vec3 Color{0.0f, 0.0f, 0.0f};
        float Quadratic = 0.0f;
    };

    struct LightGlobals {
        glm::vec3 DirectionalLight;
        float Padding0;
        glm::vec3 DirectionalColor;
        float Padding1;
        glm::vec3 AmbientColor;
        float Padding2;
        glm::vec3 CascadeShadowMapSplits;
        float Padding3;
        glm::mat4 ShadowMatrices[4];
        PointLightData PointLightData[32];
    };

public:
    NO_MOVE_OR_COPY(DeferredRenderer)

    DeferredRenderer();

    ~DeferredRenderer() override;

    void OnResize(const glm::ivec2 &size) override;

    void SetCameraInfo(const glm::mat4 &view, float fov, float near, float far) override;

    void SetWorldLight(const glm::vec3 &lightDirection, const glm::vec3 &lightColor, const glm::vec3 &ambientColor) override;

    void SetWorldBounds(const glm::vec3 &min, const glm::vec3 &max) override;

    void BeginDraw() override;

    void EndDraw() override;

    void DrawPointLight(const glm::vec3 &position, const glm::vec3 &color, float linear, float quadratic) override;

    void DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &color) override;

    void DrawMesh(const MeshBase &mesh, const glm::mat4 &model, const Material *material) override;

private:
    void FlushUniformBuffers();

    void DrawToShadowMap();

    void DrawToGBuffers();

    void DrawForwardPass();

    ShadowMap m_shadowMap{2048};
    Framebuffer m_gBuffers;

    UniformBuffer<ShaderGlobals> m_shaderGlobals;
    UniformBuffer<LightGlobals> m_lightGlobals;

    ShadowMatrixCalculator m_shadowMatrixCalculator;

    DeferredShaderShadowPass m_shadowShader;
    DeferredShaderBase m_baseShader;
    DeferredShaderGPass m_gPassShader;
    DeferredShaderLines m_linesShader;
    DeferredShaderSkybox m_skyboxShader;

    MeshPositionOnly m_fullscreenQuad;
    MeshPositionOnly m_skyboxCube;

    Texture m_defaultDiffuseTexture;
    Material m_defaultMaterial{
            &m_defaultDiffuseTexture
    };

    std::vector<PointLightData> m_pendingPointLightData;

    std::vector<VertexPositionColor> m_pendingLines;
    MeshPositionColor m_linesMesh{GL_LINES};

    struct DrawCallBase {
        const MeshBase &Mesh;
        glm::mat4 Model{1.0f};
        Material material;
    };

    std::vector<DrawCallBase> m_pendingBaseDrawCalls;
};
