//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "haru/render/DeferredShaders.h"
#include "haru/render/Framebuffer.h"
#include "haru/render/Mesh.h"
#include "haru/render/Renderer.h"
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
        float DirectionalLightIntensity;
        PointLightData PointLightData[32];
    };

public:
    MOVE_ONLY(DeferredRenderer)

    DeferredRenderer();

    ~DeferredRenderer() override;

    void Resize(const glm::ivec2 &size) override;

    void SetCameraMatrices(const glm::mat4 &view, const glm::mat4 &projection) override;

    void SetDirectionalLight(const glm::vec3 &lightDirection, float intensity) override;

    void BeginDraw() override;

    void EndDraw() override;

    void DrawPointLight(const glm::vec3 &position, const glm::vec3 &color, float linear, float quadratic) override;

    void DrawLines(const MeshPositionOnly &lines, const glm::vec4 &color) override;

    void DrawMesh(const MeshBase &mesh, const glm::mat4 &model) override;

private:
    void FlushUniformBuffers();

    void DrawToGBuffers();

    void DrawForwardPass();

    glm::ivec2 m_screenSize{};

    Framebuffer m_gBuffers;

    UniformBuffer<ShaderGlobals> m_shaderGlobals;
    UniformBuffer<LightGlobals> m_lightGlobals;

    DeferredShaderBase m_baseShader;
    DeferredShaderGPass m_gPassShader;
    DeferredShaderLines m_linesShader;

    MeshPositionOnly m_fullscreenQuad;

    std::vector<PointLightData> m_pendingPointLightData;

    struct DrawCallBase {
        const MeshBase &Mesh;
        glm::mat4 Model;
    };

    std::vector<DrawCallBase> m_pendingBaseDrawCalls;

    struct DrawCallLines {
        const MeshPositionOnly &Mesh;
        glm::vec4 Color;
    };

    std::vector<DrawCallLines> m_pendingLinesDrawCalls;
};