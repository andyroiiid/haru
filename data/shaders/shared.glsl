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
