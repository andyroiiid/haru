//
// Created by andyroiiid on 11/21/2022.
//

#include "haru/render/DeferredShaders.h"

static const char *DEFERRED_SHADERS_SHARED = R"GLSL(
layout(std140, binding = 0) uniform ShaderGlobals {
    mat4 uView;
    mat4 uProjection;
};

struct PointLightData {
	vec3 position;
	float linear;
	vec3 color;
	float quadratic;
};

layout(std140, binding = 1) uniform LightGlobals {
	vec3 uDirectionalLight;
	float uDirectionalLightIntensity;
    vec3 uCascadeShadowMapSplits;
    float Padding0;
    mat4 uShadowMatrices[4];
	PointLightData uPointLightData[32];
};
)GLSL";

DeferredShaderShadowPass::DeferredShaderShadowPass()
        : Shader(DEFERRED_SHADERS_SHARED,
                 R"GLSL(
layout(location = 0) in vec3 aPosition;

layout(location = 0) uniform mat4 uModel;

void main() {
	gl_Position = uModel * vec4(aPosition, 1);
}
)GLSL",
                 R"GLSL(
layout(triangles, invocations = 4) in;

layout(triangle_strip, max_vertices = 3) out;

void main() {
    for (int i = 0; i < 3; i++) {
        gl_Position = uShadowMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}
)GLSL",
                 R"GLSL(
void main()
{
    gl_FragDepth = gl_FragCoord.z;
}
)GLSL") {
    m_modelLocation = GetUniformLocation("uModel");
}

void DeferredShaderShadowPass::SetModel(const glm::mat4 &model) {
    SetUniform(m_modelLocation, model);
}

DeferredShaderBase::DeferredShaderBase()
        : Shader(DEFERRED_SHADERS_SHARED,
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

layout(binding = 0) uniform sampler2D uDiffuseTexture;

void main() {
	fWorldPosition = vWorldPosition;
	fWorldNormal = normalize(vWorldNormal);
	fDiffuse = texture(uDiffuseTexture, vTexCoord);
}
)GLSL") {
    m_modelLocation = GetUniformLocation("uModel");
}

void DeferredShaderBase::SetModel(const glm::mat4 &model) {
    SetUniform(m_modelLocation, model);
}

DeferredShaderGPass::DeferredShaderGPass()
        : Shader(DEFERRED_SHADERS_SHARED,
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
layout(binding = 3) uniform sampler2DArrayShadow uShadowMap;

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

float BlurShadow(vec3 shadowCoord, int layer, float bias) {
    const vec2 texelSize = 1.0 / textureSize(uShadowMap, 0).xy;
    float shadow = 0.0;
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            vec4 coord = vec4(shadowCoord.xy + vec2(x, y) * texelSize, layer, shadowCoord.z - bias);
            shadow += texture(uShadowMap, coord);
        }
    }
    shadow /= 25.0;
    return shadow;
}

int GetShadowLayer(vec4 viewSpacePos)
{
    const float depth = abs(viewSpacePos.z);
    int layer = 0;
    if (depth > uCascadeShadowMapSplits[0]) layer = 1;
    if (depth > uCascadeShadowMapSplits[1]) layer = 2;
    if (depth > uCascadeShadowMapSplits[2]) layer = 3;
    return layer;
}

float ReadShadowMap(vec4 viewSpacePos, vec4 worldPos, vec3 worldNormal)
{
    int layer = GetShadowLayer(viewSpacePos);
    vec4 lightSpacePos = uShadowMatrices[layer] * worldPos;
    vec3 shadowCoord = lightSpacePos.xyz / lightSpacePos.w;
    shadowCoord = shadowCoord * 0.5 + 0.5;
    float bias = max(0.01 * (1.0 - dot(worldNormal, uDirectionalLight)), 0.005);
    return shadowCoord.z > 1.0 ? 0.0 : BlurShadow(shadowCoord, layer, bias);
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
	vec4 worldPosition = texture(uWorldPosition, vTexCoord);
    vec4 viewSpacePosition = uView * worldPosition;
	vec3 worldNormal = normalize(texture(uWorldNormal, vTexCoord).xyz);
	vec4 diffuse = texture(uDiffuse, vTexCoord);

	vec3 lighting = vec3(0.0f);

	if (uDirectionalLightIntensity > 0) {
        float shadow = ReadShadowMap(viewSpacePosition, worldPosition, worldNormal);
		lighting += (1 - shadow) * LightDiffuse(worldNormal, uDirectionalLight) * uDirectionalLightIntensity;
	}

	for (int i = 0; i < 32; i++) {
		PointLightData pointLightData = uPointLightData[i];
		if (pointLightData.color.r > 0 || pointLightData.color.g > 0 || pointLightData.color.b > 0) {
			float intensity = LightDiffuse(worldNormal, worldPosition.xyz, pointLightData.position, pointLightData.linear, pointLightData.quadratic);
			lighting += pointLightData.color * intensity;
		}
	}

	vec4 color = diffuse;

    /* CSM splits visualization
    int layer = GetShadowLayer(viewSpacePosition);
    vec3 layerVis[4] = vec3[](
        vec3(1.0, 1.0, 1.0),
        vec3(1.0, 0.8, 0.8),
        vec3(0.8, 1.0, 0.8),
        vec3(0.8, 0.8, 1.0)
    );
    color.rgb = layerVis[layer];
    */

	color.rgb *= lighting;
	color.rgb = ACESToneMapping(color.rgb);

	fColor = color;
}
)GLSL") {
}

DeferredShaderLines::DeferredShaderLines()
        : Shader(DEFERRED_SHADERS_SHARED,
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

DeferredShaderSkybox::DeferredShaderSkybox()
        : Shader(DEFERRED_SHADERS_SHARED,
                 R"GLSL(
layout(location = 0) in vec3 aPosition;

layout(location = 0) out vec3 vPosition;

void main() {
    mat4 view = uView;
    view[3] = vec4(0, 0, 0, 1);
    vec4 position = uProjection * view * vec4(aPosition, 1);
	gl_Position = position.xyww;
    vPosition = aPosition;
}
)GLSL",
                 R"GLSL(
layout(location = 0) in vec3 vPosition;

layout(location = 0) out vec4 fColor;

// https://github.com/wwwtyro/glsl-atmosphere

#define PI 3.141592
#define iSteps 16
#define jSteps 8

vec2 rsi(vec3 r0, vec3 rd, float sr) {
    // ray-sphere intersection that assumes
    // the sphere is centered at the origin.
    // No intersection when result.x > result.y
    float a = dot(rd, rd);
    float b = 2.0 * dot(rd, r0);
    float c = dot(r0, r0) - (sr * sr);
    float d = (b*b) - 4.0*a*c;
    if (d < 0.0) return vec2(1e5, -1e5);
    return vec2(
        (-b - sqrt(d))/(2.0*a),
        (-b + sqrt(d))/(2.0*a)
    );
}

vec3 atmosphere(vec3 r, vec3 r0, vec3 pSun, float iSun, float rPlanet, float rAtmos, vec3 kRlh, float kMie, float shRlh, float shMie, float g) {
    // Normalize the sun and view directions.
    pSun = normalize(pSun);
    r = normalize(r);
    // Calculate the step size of the primary ray.
    vec2 p = rsi(r0, r, rAtmos);
    if (p.x > p.y) return vec3(0, 0, 0);
    p.y = min(p.y, rsi(r0, r, rPlanet).x);
    float iStepSize = (p.y - p.x) / float(iSteps);
    // Initialize the primary ray time.
    float iTime = 0.0;
    // Initialize accumulators for Rayleigh and Mie scattering.
    vec3 totalRlh = vec3(0, 0, 0);
    vec3 totalMie = vec3(0, 0, 0);
    // Initialize optical depth accumulators for the primary ray.
    float iOdRlh = 0.0;
    float iOdMie = 0.0;
    // Calculate the Rayleigh and Mie phases.
    float mu = dot(r, pSun);
    float mumu = mu * mu;
    float gg = g * g;
    float pRlh = 3.0 / (16.0 * PI) * (1.0 + mumu);
    float pMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));
    // Sample the primary ray.
    for (int i = 0; i < iSteps; i++) {
        // Calculate the primary ray sample position.
        vec3 iPos = r0 + r * (iTime + iStepSize * 0.5);
        // Calculate the height of the sample.
        float iHeight = length(iPos) - rPlanet;
        // Calculate the optical depth of the Rayleigh and Mie scattering for this step.
        float odStepRlh = exp(-iHeight / shRlh) * iStepSize;
        float odStepMie = exp(-iHeight / shMie) * iStepSize;
        // Accumulate optical depth.
        iOdRlh += odStepRlh;
        iOdMie += odStepMie;
        // Calculate the step size of the secondary ray.
        float jStepSize = rsi(iPos, pSun, rAtmos).y / float(jSteps);
        // Initialize the secondary ray time.
        float jTime = 0.0;
        // Initialize optical depth accumulators for the secondary ray.
        float jOdRlh = 0.0;
        float jOdMie = 0.0;
        // Sample the secondary ray.
        for (int j = 0; j < jSteps; j++) {
            // Calculate the secondary ray sample position.
            vec3 jPos = iPos + pSun * (jTime + jStepSize * 0.5);
            // Calculate the height of the sample.
            float jHeight = length(jPos) - rPlanet;
            // Accumulate the optical depth.
            jOdRlh += exp(-jHeight / shRlh) * jStepSize;
            jOdMie += exp(-jHeight / shMie) * jStepSize;
            // Increment the secondary ray time.
            jTime += jStepSize;
        }
        // Calculate attenuation.
        vec3 attn = exp(-(kMie * (iOdMie + jOdMie) + kRlh * (iOdRlh + jOdRlh)));
        // Accumulate scattering.
        totalRlh += odStepRlh * attn;
        totalMie += odStepMie * attn;
        // Increment the primary ray time.
        iTime += iStepSize;
    }
    // Calculate and return the final color.
    return iSun * (pRlh * kRlh * totalRlh + pMie * kMie * totalMie);
}

void main()
{
    vec3 color = atmosphere(
        vPosition, // ray direction
        vec3(0, 6372e3, 0), // ray origin
        uDirectionalLight, // position of the sun
        22.0, // intensity of the sun
        6371e3, // radius of the planet in meters
        6471e3, // radius of the atmosphere in meters
        vec3(5.5e-6, 13.0e-6, 22.4e-6), // Rayleigh scattering coefficient
        21e-6, // Mie scattering coefficient
        8e3, // Rayleigh scale height
        1.2e3, // Mie scale height
        0.758// Mie preferred scattering direction
    );
    color = 1.0 - exp(-1.0 * color);
    fColor = vec4(color, 1);
}
)GLSL") {
}
