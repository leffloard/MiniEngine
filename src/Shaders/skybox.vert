#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uView;
uniform mat4 uProj;

out vec3 vWorldDir;

void main()
{
    vWorldDir = aPos;
    vec4 clip = uProj * uView * vec4(aPos, 1.0);
    gl_Position = clip.xyww;  // z = w so after divide depth = 1.0 (far plane)
}
