#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec4 aModel0;
layout (location = 4) in vec4 aModel1;
layout (location = 5) in vec4 aModel2;
layout (location = 6) in vec4 aModel3;

uniform mat4 uView;
uniform mat4 uProj;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUV;

void main()
{
    mat4 uModel = mat4(aModel0, aModel1, aModel2, aModel3);
    vec4 world = uModel * vec4(aPos, 1.0);
    vWorldPos = world.xyz;
    mat3 normalMat = mat3(transpose(inverse(uModel)));
    vNormal = normalize(normalMat * aNormal);
    vUV = aUV;
    gl_Position = uProj * uView * world;
}
