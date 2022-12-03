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

float BlurShadow(vec3 shadowCoord, int layer) {
    const vec2 texelSize = 1.0 / textureSize(uShadowMap, 0).xy;
    float shadow = 0.0;
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            vec4 coord = vec4(shadowCoord.xy + vec2(x, y) * texelSize, layer, shadowCoord.z);
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
    return shadowCoord.z > 1.0 ? 0.0 : BlurShadow(shadowCoord, layer);
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

    vec3 lighting = uAmbientColor;

    float shadow = ReadShadowMap(viewSpacePosition, worldPosition, worldNormal);
    lighting += (1 - shadow) * LightDiffuse(worldNormal, uDirectionalLight) * uDirectionalColor;

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
