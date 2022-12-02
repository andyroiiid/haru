layout(location = 0) in vec3 aPosition;

layout(location = 0) uniform mat4 uModel;

void main() {
    gl_Position = uModel * vec4(aPosition, 1);
}
