//
// Created by andyroiiid on 11/21/2022.
//

#include "haru/render/DeferredShaders.h"

DeferredShaderBase::DeferredShaderBase()
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

void DeferredShaderBase::SetModel(const glm::mat4 &model) {
    SetUniform(m_modelLocation, model);
}

DeferredShaderGPass::DeferredShaderGPass()
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

DeferredShaderLines::DeferredShaderLines()
        : Shader(R"GLSL(
layout(std140, binding = 0) uniform ShaderGlobals {
    mat4 uView;
    mat4 uProjection;
};
)GLSL",
                 R"GLSL(
layout(location = 0) in vec3 aPosition;

void main() {
	gl_Position = uProjection * uView * vec4(aPosition, 1);
}
)GLSL",
                 R"GLSL(
layout(location = 0) out vec4 fColor;

layout(location = 0) uniform vec4 uColor;

void main() {
    fColor = uColor;
}
)GLSL") {
    m_colorLocation = GetUniformLocation("uColor");
}

void DeferredShaderLines::SetColor(const glm::vec4 &color) {
    SetUniform(m_colorLocation, color);
}
