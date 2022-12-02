layout(location = 0) in vec3 aPosition;

void main() {
    gl_Position = uProjection * uView * vec4(aPosition, 1);
}
