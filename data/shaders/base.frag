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
