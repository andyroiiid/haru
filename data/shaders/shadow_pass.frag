layout(location = 0) in vec3 gWorldNormal;

void main()
{
    float slopeScale = 1 - abs(dot(uDirectionalLight, gWorldNormal));
    gl_FragDepth = gl_FragCoord.z + 0.002 * slopeScale;
}
