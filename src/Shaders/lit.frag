#version 330 core
out vec4 FragColor;

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;

uniform vec3 uViewPos;
uniform vec3 uBaseColor;
uniform float uMetal;
uniform float uRough;
uniform int uIsGround;
uniform int uGridEnabled;

uniform sampler2D uDiffuseTex;
uniform int uUseTexture;

// Directional + point light
uniform vec3 uDirLightDir;
uniform vec3 uDirLightColor;
uniform vec3 uPointLightPos;
uniform vec3 uPointLightColor;
uniform float uSpecularIntensity;

// Blinn-Phong contribution from one light direction and color
vec3 blinnPhong(vec3 n, vec3 viewDir, vec3 lightDir, vec3 lightColor, vec3 albedo) {
    float diff = max(dot(n, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * albedo;
    vec3 H = normalize(lightDir + viewDir);
    float spec = pow(max(dot(n, H), 0.0), 32.0);
    vec3 specular = uSpecularIntensity * (1.0 - uRough) * spec * lightColor;
    return diffuse + specular;
}

void main()
{
    vec3 albedo = uBaseColor;
    if (uUseTexture == 1) {
        albedo = texture(uDiffuseTex, vUV).rgb;
    }

    vec3 n = normalize(vNormal);
    vec3 viewDir = normalize(uViewPos - vWorldPos);

    // Ambient (constant)
    vec3 ambient = 0.12 * albedo;

    // Directional light (uDirLightDir = direction light is coming FROM; we need direction TO light)
    vec3 dirLightDir = normalize(-uDirLightDir);
    vec3 dirContrib = blinnPhong(n, viewDir, dirLightDir, uDirLightColor, albedo);

    // Point light with attenuation
    vec3 toPoint = uPointLightPos - vWorldPos;
    float dist = length(toPoint);
    vec3 pointLightDir = toPoint / max(dist, 0.001);
    float att = 1.0 / (1.0 + 0.1 * dist + 0.02 * dist * dist);
    vec3 pointContrib = att * blinnPhong(n, viewDir, pointLightDir, uPointLightColor, albedo);

    vec3 color = ambient + dirContrib + pointContrib;

    // Procedural grid on ground
    if (uIsGround == 1 && uGridEnabled == 1) {
        vec2 coord = vWorldPos.xz;
        vec2 grid = abs(fract(coord - 0.5) - 0.5);
        vec2 dxy = fwidth(coord);
        vec2 gridLine = smoothstep(dxy, dxy * 2.0, grid);
        float line = min(gridLine.x, gridLine.y);
        color = mix(vec3(0.15, 0.2, 0.18), color, 1.0 - line * 0.7);
    }

    // Linear output (gamma correction done by GL_FRAMEBUFFER_SRGB)
    FragColor = vec4(color, 1.0);
}
