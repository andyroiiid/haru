//
// Created by andyroiiid on 11/20/2022.
//

#include "haru/DeferredRenderer.h"

DeferredRenderer::ShaderBase::ShaderBase()
        : Shader(R"GLSL(
layout(std140, binding = 0) uniform ShaderGlobals {
    mat4 uView;
    mat4 uProjection;
};
)GLSL",
                 R"GLSL(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(location = 0) out vec4 vWorldPosition;
layout(location = 1) out vec4 vWorldNormal;
layout(location = 2) out vec2 vTexCoord;

layout(location = 0) uniform mat4 uModel;

void main() {
	gl_Position = uProjection * uView * uModel * vec4(aPosition, 1);
	vWorldPosition = uModel * vec4(aPosition, 1);
	vWorldNormal = uModel * vec4(aNormal, 0);
	vTexCoord = aTexCoord;
}
)GLSL",
                 R"GLSL(
layout(location = 0) in vec4 vWorldPosition;
layout(location = 1) in vec4 vWorldNormal;
layout(location = 2) in vec2 vTexCoord;

layout(location = 0) out vec4 fWorldPosition;
layout(location = 1) out vec4 fWorldNormal;
layout(location = 2) out vec4 fDiffuse;

void main() {
	fWorldPosition = vWorldPosition;
	fWorldNormal = normalize(vWorldNormal);
	fDiffuse = vec4(1, 1, 1, 1);
}
)GLSL") {
    m_modelLocation = GetUniformLocation("uModel");
}

void DeferredRenderer::ShaderBase::SetModel(const glm::mat4 &model) {
    SetUniform(m_modelLocation, model);
}

DeferredRenderer::ShaderGPass::ShaderGPass()
        : Shader(R"GLSL(
struct PointLightData {
	vec3 position;
	float linear;
	vec3 color;
	float quadratic;
};

layout(std140, binding = 1) uniform LightGlobals {
	vec3 uDirectionalLight;
	float uDirectionalLightIntensity;
	PointLightData uPointLightData[32];
};
)GLSL",
                 R"GLSL(
layout(location = 0) in vec2 aPosition;

layout(location = 0) out vec2 vTexCoord;

void main() {
	gl_Position = vec4(aPosition, 0, 1);
	vTexCoord = aPosition * 0.5f + 0.5f;
}
)GLSL",
                 R"GLSL(
layout(location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 fColor;

layout(binding = 0) uniform sampler2D uWorldPosition;
layout(binding = 1) uniform sampler2D uWorldNormal;
layout(binding = 2) uniform sampler2D uDiffuse;

float LightDiffuse(vec3 worldNormal, vec3 lightDirection) {
    return max(0, dot(worldNormal, normalize(lightDirection)));
}

float LightDiffuse(vec3 worldNormal, vec3 worldPosition, vec3 lightPosition, float linear, float quadratic)
{
	const vec3 delta = lightPosition - worldPosition;
	const float diffuse = LightDiffuse(worldNormal, delta);
	const float dist = length(delta);
	// point light attenuation from https://wiki.ogre3d.org/-Point+Light+Attenuation
	const float attenuation = 1.0 + linear * dist + quadratic * dist * dist;
	return diffuse / attenuation;
}

vec3 ACESToneMapping(vec3 color) {
	const float A = 2.51;
	const float B = 0.03;
	const float C = 2.43;
	const float D = 0.59;
	const float E = 0.14;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}

void main() {
	vec3 worldPosition = texture(uWorldPosition, vTexCoord).xyz;
	vec3 worldNormal = normalize(texture(uWorldNormal, vTexCoord).xyz);
	vec4 diffuse = texture(uDiffuse, vTexCoord);

	vec3 lighting = vec3(0.0f);

	if (uDirectionalLightIntensity > 0) {
		lighting += LightDiffuse(worldNormal, uDirectionalLight) * uDirectionalLightIntensity;
	}

	for (int i = 0; i < 32; i++) {
		PointLightData pointLightData = uPointLightData[i];
		if (pointLightData.color.r > 0 || pointLightData.color.g > 0 || pointLightData.color.b > 0) {
			float intensity = LightDiffuse(worldNormal, worldPosition, pointLightData.position, pointLightData.linear, pointLightData.quadratic);
			lighting += pointLightData.color * intensity;
		}
	}

	vec4 color = diffuse;
	color.rgb *= lighting;
	color.rgb = ACESToneMapping(color.rgb);

	fColor = color;
}
)GLSL") {
}

DeferredRenderer::DeferredRenderer() {
    m_shaderGlobals.Bind(0);
    m_lightGlobals.Bind(1);

    m_fullscreenQuad = Mesh<VertexPositionOnly>(
            {
                    {{-1.0f, -1.0f}},
                    {{1.0f,  -1.0f}},
                    {{-1.0f, 1.0f}},
                    {{1.0f,  1.0f}},
            },
            GL_TRIANGLE_STRIP
    );
}

DeferredRenderer::~DeferredRenderer() {
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, 0);
}

void DeferredRenderer::Resize(const glm::ivec2 &size) {
    m_screenSize = size;

    m_gBuffers = Framebuffer(
            size,
            {
                    GL_RGBA32F, // WorldPosition
                    GL_RGBA32F, // WorldNormal
                    GL_RGBA8 // Color
            }
    );
}

void DeferredRenderer::SetCameraMatrices(const glm::mat4 &view, const glm::mat4 &projection) {
    m_shaderGlobals->View = view;
    m_shaderGlobals->Projection = projection;
}

void DeferredRenderer::SetDirectionalLight(const glm::vec3 &lightDirection, const float intensity) {
    m_lightGlobals->DirectionalLight = lightDirection;
    m_lightGlobals->DirectionalLightIntensity = intensity;
}

void DeferredRenderer::BeginDraw() {
    m_pendingBaseDrawCalls.clear();
    m_pendingPointLightData.clear();
}

void DeferredRenderer::EndDraw() {
    m_shaderGlobals.Flush();

    const size_t numPointLights = m_pendingPointLightData.size();

    for (size_t i = 0; i < 32; i++) {
        m_lightGlobals->PointLightData[i] = i < numPointLights ? m_pendingPointLightData[i] : PointLightData{};
    }

    m_lightGlobals.Flush();

    DrawToGBuffers();

    Framebuffer::BindDefault();
    glViewport(0, 0, m_screenSize.x, m_screenSize.y);

    constexpr GLfloat clearColor[4]{0.0f, 0.0f, 0.0f, 1.0f};
    glClearBufferfv(GL_COLOR, 0, clearColor);
    glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

    m_gPassShader.Use();
    m_gBuffers.BindAllTextures();
    m_fullscreenQuad.BindAndDraw();
    glBindVertexArray(0);
    m_gBuffers.UnbindAllTextures();
    glUseProgram(0);
}

void DeferredRenderer::DrawPointLight(const glm::vec3 &position, const glm::vec3 &color, const float linear,
                                      const float quadratic) {
    m_pendingPointLightData.push_back({position, linear, color, quadratic});
}

void DeferredRenderer::DrawMesh(const MeshBase &mesh, const glm::mat4 &model) {
    m_pendingBaseDrawCalls.push_back({mesh, model});
}

void DeferredRenderer::DrawToGBuffers() {
    m_gBuffers.Bind();
    glViewport(0, 0, m_gBuffers.Size().x, m_gBuffers.Size().y);

    m_gBuffers.Clear();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_baseShader.Use();
    for (const auto &baseDrawCall: m_pendingBaseDrawCalls) {
        m_baseShader.SetModel(baseDrawCall.Model);
        baseDrawCall.Mesh.BindAndDraw();
    }

    glBindVertexArray(0);
    glBindTextureUnit(0, 0);
    glUseProgram(0);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}