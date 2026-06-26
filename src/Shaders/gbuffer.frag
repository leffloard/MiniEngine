#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec2 gMaterial;

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;

uniform vec3 uBaseColor;
uniform float uMetal;
uniform float uRough;
uniform int uIsGround;
uniform int uGridEnabled;

uniform sampler2D uDiffuseTex;
uniform int uUseTexture;

void main()
{
    gPosition = vWorldPos;
    gNormal = normalize(vNormal);

    vec3 albedo = uBaseColor;
    if (uUseTexture == 1)
        albedo = texture(uDiffuseTex, vUV).rgb;

    float isGround = (uIsGround == 1 && uGridEnabled == 1) ? 1.0 : 0.0;
    gAlbedo = vec4(albedo, isGround);

    gMaterial = vec2(uMetal, uRough);

    if (isGround > 0.5) {
        vec2 coord = vWorldPos.xz;
        vec2 grid = abs(fract(coord - 0.5) - 0.5);
        vec2 dxy = fwidth(coord);
        vec2 gridLine = smoothstep(dxy, dxy * 2.0, grid);
        float line = min(gridLine.x, gridLine.y);
        gAlbedo.rgb = mix(vec3(0.15, 0.2, 0.18), gAlbedo.rgb, 1.0 - line * 0.7);
    }
}
