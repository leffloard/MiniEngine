#version 330 core
out vec4 FragColor;

in vec3 vWorldDir;

uniform samplerCube uSkybox;

void main()
{
    FragColor = vec4(texture(uSkybox, normalize(vWorldDir)).rgb, 1.0);
}
