//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "haru/Framebuffer.h"
#include "haru/UniformBuffer.h"
#include "haru/Shader.h"
#include "haru/Mesh.h"
#include "haru/Renderer.h"

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

    class ShaderBase : public Shader {
    public:
        MOVE_ONLY(ShaderBase)

        ShaderBase();

        void SetModel(const glm::mat4 &model);

    private:
        GLint m_modelLocation = -1;
    };

    struct VertexPositionOnly {
        glm::vec2 Position;

        static void SetupVertexArray(const GLuint vao) {
            SetupVertexArrayFloatsAttrib(vao, 0, 0, 2, offsetof(VertexPositionOnly, Position));
        }
    };

    class ShaderGPass : public Shader {
    public:
        MOVE_ONLY(ShaderGPass)

        ShaderGPass();
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

    void DrawMesh(const MeshBase &mesh, const glm::mat4 &model) override;

private:
    void DrawToGBuffers();

    glm::ivec2 m_screenSize{};

    Framebuffer m_gBuffers;
    UniformBuffer<ShaderGlobals> m_shaderGlobals;
    UniformBuffer<LightGlobals> m_lightGlobals;

    ShaderBase m_baseShader;
    ShaderGPass m_gPassShader;

    Mesh<VertexPositionOnly> m_fullscreenQuad;

    struct DrawCallBase {
        const MeshBase &Mesh;
        glm::mat4 Model;
    };

    std::vector<DrawCallBase> m_pendingBaseDrawCalls;

    std::vector<PointLightData> m_pendingPointLightData;
};