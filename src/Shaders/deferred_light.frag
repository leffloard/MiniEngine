#version 330 core
out vec4 FragColor;

in vec2 vTexCoord;

uniform vec3 uViewPos;
uniform vec3 uDirLightDir;
uniform vec3 uDirLightColor;
uniform vec3 uPointLightPos;
uniform vec3 uPointLightColor;
uniform float uSpecularIntensity;
uniform int uGridEnabled;

uniform sampler2D uGPosition;
uniform sampler2D uGNormal;
uniform sampler2D uGAlbedo;
uniform sampler2D uGMaterial;

vec3 blinnPhong(vec3 n, vec3 viewDir, vec3 lightDir, vec3 lightColor, vec3 albedo, float rough, float metal) {
    float diff = max(dot(n, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * albedo;
    vec3 H = normalize(lightDir + viewDir);
    float spec = pow(max(dot(n, H), 0.0), 32.0);
    float specStrength = uSpecularIntensity * (1.0 - rough);
    vec3 specular = specStrength * spec * lightColor;
    return diffuse + specular;
}

void main()
{
    vec3 worldPos = texture(uGPosition, vTexCoord).rgb;
    vec3 n = normalize(texture(uGNormal, vTexCoord).rgb);
    vec4 albedoPack = texture(uGAlbedo, vTexCoord);
    vec3 albedo = albedoPack.rgb;
    float isGround = albedoPack.a;
    vec4 matPack = texture(uGMaterial, vTexCoord);
    float metal = matPack.r;
    float rough = matPack.g;

    vec3 viewDir = normalize(uViewPos - worldPos);

    vec3 ambient = 0.12 * albedo;

    vec3 dirLightDir = normalize(-uDirLightDir);
    vec3 dirContrib = blinnPhong(n, viewDir, dirLightDir, uDirLightColor, albedo, rough, metal);

    vec3 toPoint = uPointLightPos - worldPos;
    float dist = length(toPoint);
    vec3 pointLightDir = toPoint / max(dist, 0.001);
    float att = 1.0 / (1.0 + 0.1 * dist + 0.02 * dist * dist);
    vec3 pointContrib = att * blinnPhong(n, viewDir, pointLightDir, uPointLightColor, albedo, rough, metal);

    vec3 color = ambient + dirContrib + pointContrib;

    FragColor = vec4(color, 1.0);
}
